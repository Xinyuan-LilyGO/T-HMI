// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2023, Benoit BLANCHON
// MIT License

#include <ArduinoJson.h>
#include <stdlib.h>  // malloc, free
#include <catch.hpp>
#include <sstream>
#include <utility>

class SpyingAllocator {
 public:
  SpyingAllocator(const SpyingAllocator& src) : _log(src._log) {}
  SpyingAllocator(std::ostream& log) : _log(log) {}
  SpyingAllocator& operator=(const SpyingAllocator& src) = delete;

  void* allocate(size_t n) {
    _log << "A" << n;
    return malloc(n);
  }
  void deallocate(void* p) {
    _log << "F";
    free(p);
  }

 private:
  std::ostream& _log;
};

class ControllableAllocator {
 public:
  ControllableAllocator() : _enabled(true) {}

  void* allocate(size_t n) {
    return _enabled ? malloc(n) : 0;
  }

  void deallocate(void* p) {
    free(p);
  }

  void disable() {
    _enabled = false;
  }

 private:
  bool _enabled;
};

TEST_CASE("BasicJsonDocument") {
  std::stringstream log;

  SECTION("Construct/Destruct") {
    { BasicJsonDocument<SpyingAllocator> doc(4096, log); }
    REQUIRE(log.str() == "A4096F");
  }

  SECTION("Copy construct") {
    {
      BasicJsonDocument<SpyingAllocator> doc1(4096, log);
      doc1.set(std::string("The size of this string is 32!!"));

      BasicJsonDocument<SpyingAllocator> doc2(doc1);

      REQUIRE(doc1.as<std::string>() == "The size of this string is 32!!");
      REQUIRE(doc2.as<std::string>() == "The size of this string is 32!!");
      REQUIRE(doc2.capacity() == 4096);
    }
    REQUIRE(log.str() == "A4096A4096FF");
  }

  SECTION("Move construct") {
    {
      BasicJsonDocument<SpyingAllocator> doc1(4096, log);
      doc1.set(std::string("The size of this string is 32!!"));

      BasicJsonDocument<SpyingAllocator> doc2(std::move(doc1));

      REQUIRE(doc2.as<std::string>() == "The size of this string is 32!!");
      REQUIRE(doc1.as<std::string>() == "null");
      REQUIRE(doc1.capacity() == 0);
      REQUIRE(doc2.capacity() == 4096);
    }
    REQUIRE(log.str() == "A4096F");
  }

  SECTION("Copy assign larger") {
    {
      BasicJsonDocument<SpyingAllocator> doc1(4096, log);
      doc1.set(std::string("The size of this string is 32!!"));
      BasicJsonDocument<SpyingAllocator> doc2(8, log);

      doc2 = doc1;

      REQUIRE(doc1.as<std::string>() == "The size of this string is 32!!");
      REQUIRE(doc2.as<std::string>() == "The size of this string is 32!!");
      REQUIRE(doc2.capacity() == 4096);
    }
    REQUIRE(log.str() == "A4096A8FA4096FF");
  }

  SECTION("Copy assign smaller") {
    {
      BasicJsonDocument<SpyingAllocator> doc1(1024, log);
      doc1.set(std::string("The size of this string is 32!!"));
      BasicJsonDocument<SpyingAllocator> doc2(4096, log);

      doc2 = doc1;

      REQUIRE(doc1.as<std::string>() == "The size of this string is 32!!");
      REQUIRE(doc2.as<std::string>() == "The size of this string is 32!!");
      REQUIRE(doc2.capacity() == 1024);
    }
    REQUIRE(log.str() == "A1024A4096FA1024FF");
  }

  SECTION("Copy assign same size") {
    {
      BasicJsonDocument<SpyingAllocator> doc1(1024, log);
      doc1.set(std::string("The size of this string is 32!!"));
      BasicJsonDocument<SpyingAllocator> doc2(1024, log);

      doc2 = doc1;

      REQUIRE(doc1.as<std::string>() == "The size of this string is 32!!");
      REQUIRE(doc2.as<std::string>() == "The size of this string is 32!!");
      REQUIRE(doc2.capacity() == 1024);
    }
    REQUIRE(log.str() == "A1024A1024FF");
  }

  SECTION("Move assign") {
    {
      BasicJsonDocument<SpyingAllocator> doc1(4096, log);
      doc1.set(std::string("The size of this string is 32!!"));
      BasicJsonDocument<SpyingAllocator> doc2(8, log);

      doc2 = std::move(doc1);

      REQUIRE(doc2.as<std::string>() == "The size of this string is 32!!");
      REQUIRE(doc1.as<std::string>() == "null");
      REQUIRE(doc1.capacity() == 0);
      REQUIRE(doc2.capacity() == 4096);
    }
    REQUIRE(log.str() == "A4096A8FF");
  }

  SECTION("garbageCollect()") {
    BasicJsonDocument<ControllableAllocator> doc(4096);

    SECTION("when allocation succeeds") {
      deserializeJson(doc, "{\"blanket\":1,\"dancing\":2}");
      REQUIRE(doc.capacity() == 4096);
      REQUIRE(doc.memoryUsage() == JSON_OBJECT_SIZE(2) + 16);
      doc.remove("blanket");

      bool result = doc.garbageCollect();

      REQUIRE(result == true);
      REQUIRE(doc.memoryUsage() == JSON_OBJECT_SIZE(1) + 8);
      REQUIRE(doc.capacity() == 4096);
      REQUIRE(doc.as<std::string>() == "{\"dancing\":2}");
    }

    SECTION("when allocation fails") {
      deserializeJson(doc, "{\"blanket\":1,\"dancing\":2}");
      REQUIRE(doc.capacity() == 4096);
      REQUIRE(doc.memoryUsage() == JSON_OBJECT_SIZE(2) + 16);
      doc.remove("blanket");
      doc.allocator().disable();

      bool result = doc.garbageCollect();

      REQUIRE(result == false);
      REQUIRE(doc.memoryUsage() == JSON_OBJECT_SIZE(2) + 16);
      REQUIRE(doc.capacity() == 4096);
      REQUIRE(doc.as<std::string>() == "{\"dancing\":2}");
    }
  }
}
