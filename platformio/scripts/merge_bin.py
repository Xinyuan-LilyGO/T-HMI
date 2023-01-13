import os
import sys
import time
from os.path import join

Import("env")

try:
    import esptool
except:
    sys.path.append(join(env['UPLOADER'], '..'))
    import esptool

# for item in env.Dictionary():
#     print('%s : %s' % (item, env[item]))
#     print()

def merge_bin_files(env):
    upload_cmd = env['UPLOADERFLAGS']
    print(upload_cmd)
    chip_type = env['BOARD_MCU']
    flash_size = upload_cmd[upload_cmd.index('--flash_size') + 1]
    flash_mode = env['BOARD_FLASH_MODE']

    OUTPUT_DIR = join(env['PROJECT_BUILD_DIR'], '..', 'bin')

    if not os.path.exists(OUTPUT_DIR):
        os.mkdir(OUTPUT_DIR)

    outputFilename = join(
                            OUTPUT_DIR,
                            "{0}_{1}_{2}.bin".format(
                                os.path.basename(env['PROJECT_DIR']),
                                env['PIOENV'],
                                time.strftime("%Y%m%d_%H%M%S", time.localtime())
                            )
                         )

    commands = []
    commands.append('--chip')
    commands.append(chip_type)
    commands.append('merge_bin')
    commands.append('-o')
    commands.append(outputFilename)
    commands.append('--flash_size')
    commands.append(flash_size)
    commands.append('--flash_mode')
    commands.append('dio')

    print("extra images:")
    for item in env['FLASH_EXTRA_IMAGES']:
        print("%s: %s" % (item[0], item[1]))

    print("%s: %s" % (env['ESP32_APP_OFFSET'], join(env['PROJECT_BUILD_DIR'], env['PIOENV'], '{}.bin'.format(env['PROGNAME']))))
    for item in env['FLASH_EXTRA_IMAGES']:
        commands.append(item[0])
        commands.append(upload_cmd[upload_cmd.index(item[0]) + 1])
    commands.append(env['ESP32_APP_OFFSET'])
    commands.append(join(env['PROJECT_BUILD_DIR'], env['PIOENV'], '{}.bin'.format(env['PROGNAME'])))
    esptool.main(commands)


def before_upload(source, target, env):
    merge_bin_files(env)

def after_buildprog(source, target, env):
    merge_bin_files(env)

env.AddPostAction("buildprog", after_buildprog)
# env.AddPreAction("upload", before_upload)
# env.AddPostAction("$PROGPATH", after_buildprog)
# env.AddPostAction("$LINK", after_buildprog)
