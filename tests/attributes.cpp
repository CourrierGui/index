#include <clean-test/clean-test.h>

#include <iostream>

#include <db.hpp>
#include <SQLiteCpp/SQLiteCpp.h>

namespace ct = clean_test;
using namespace ct::literals;

auto const suite = ct::Suite{"Index", [] {
    try {
        SQLite::Database db{"test.db", SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE};

        if (db.tableExists("FileIndex")) {
            db.exec("drop table Files");
            db.exec("drop table FileIndex");
        }
        // FIXME check what happens if the length of the arguments is greater than 255
        // FIXME set file_name length to PATH_MAX
        db.exec("create table Files ( file_name varchar(255), primary key (file_name) )");
        db.exec("create table FileIndex ( "
                "key varchar(255), "
                "value varchar(255), "
                "file_name varchar(255), "
                "primary key (key, value, file_name) )");
    } catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        ct::expect(false);
    }

    "create index"_test = [] {
        try {
            idx::index idx("test.db");

            ct::expect(idx.files().size() == 0_i);
        } catch (std::exception& e) {
            std::cerr << e.what() << '\n';
            ct::expect(false);
        }
    };

    "insert file"_test = [] {
        try {
            idx::index idx("test.db");

            idx.insert("test.txt");

            ct::expect(idx.files().size() == 1_i);
            ct::expect(idx.files().front() == "test.txt"_sv);
        } catch (std::exception& e) {
            std::cerr << e.what() << '\n';
            ct::expect(false);
        }
    };

    "delete file"_test = [] {
        try {
            idx::index idx("test.db");

            idx.remove("test.txt");
            ct::expect(idx.files().size() == 0_i);
        } catch (std::exception& e) {
            std::cerr << e.what() << '\n';
            ct::expect(false);
        }
    };

    "ensure no duplicates"_test = [] {
        try {
            idx::index idx("test.db");

            idx.insert("test.txt");
            idx.insert("test.txt");

            {
                auto res = idx.find({  });
                ct::expect(res.size() == 1_i);
                ct::expect(res.front() == "test.txt"_sv);
            }

            idx.remove("test.txt");

            idx.insert("test.txt", { { "k", "v" } });
            idx.insert("test.txt", { { "k", "v" } });

            {
                auto res = idx.find({ });
                ct::expect(res.size() == 1_i);
                ct::expect(res.front() == "test.txt"_sv);
            }

            {
                auto res = idx.find({ { "k", "v" }});
                ct::expect(res.size() == 1_i);
                ct::expect(res.front() == "test.txt"_sv);
            }

        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            ct::expect(false);
        }
    };

    "insert file with attribute"_test = [] {
        try {
            idx::index idx{"test.db"};

            idx.insert("test.txt", { { "key", "value" } });
            ct::expect(idx.files().front() == "test.txt");

            {
                auto res = idx.find({ { "key", "" } });
                ct::expect(res.size() == 1_i);
                ct::expect(res.front() == "test.txt"_sv);
            }

            {
                auto res = idx.find({ { "key", "value" } });
                ct::expect(res.size() == 1_i);
                ct::expect(res.front() == "test.txt"_sv);
            }

            {
                auto res = idx.find({ { "not found", "value" } });
                ct::expect(res.size() == 0_i);
            }

            {
                auto res = idx.find({ { "key", "not found" } });
                ct::expect(res.size() == 0_i);
            }
            idx.remove("test.txt");
        } catch (std::exception& e) {
            std::cerr << e.what() << '\n';
            ct::expect(false);
        }
    };

    "insert file with multiple attributes"_test = [] {
        try {
            idx::index idx{"test.db"};

            idx.insert("test.txt",
                       {
                           { "key", "value" },
                           { "key2", "value2" }
                       });
            ct::expect(idx.files().front() == "test.txt");

            {
                auto res = idx.find({ { "key", "" }, { "key2", "" } });
                ct::expect(res.size() == 1_i);
                ct::expect(res.front() == "test.txt"_sv);
            }

            {
                auto res = idx.find({ { "key", "value" }, { "key2", "value2" } });
                ct::expect(res.size() == 1_i);
                ct::expect(res.front() == "test.txt"_sv);
            }

            {
                auto res = idx.find({ { "key", "not found" }, { "key2", "value2" } });
                ct::expect(res.size() == 0_i);
            }

            {
                auto res = idx.find({ { "not found", "value" }, { "key2", "value2" } });
                ct::expect(res.size() == 0_i);
            }
            idx.remove("test.txt");
        } catch (std::exception& e) {
            std::cerr << e.what() << '\n';
            ct::expect(false);
        }
    };

    "list all files"_test = [] {
        try {
            idx::index idx{"test.db"};

            idx.insert("test1.txt", { { "k", "v" } });
            idx.insert("test1.txt", { { "k1", "v1" } });
            idx.insert("test2.txt", { { "k", "v" } });
            idx.insert("test3.txt", { { "k", "v" } });

            {
                auto res = idx.find({ });
                ct::expect(res.size() == 3_i);
            }

            idx.insert("test4.txt");
            {
                auto res = idx.find({ });
                ct::expect(res.size() == 4_i);
            }

            idx.remove("test.txt");
            idx.remove("test2.txt");
            idx.remove("test3.txt");
            idx.remove("test4.txt");
        } catch (std::exception& e) {
            std::cerr << e.what() << '\n';
            ct::expect(false);
        }
    };

    "update value"_test = [] {
        try {
            idx::index idx("test.db");

            idx.insert("test.txt", { { "key", "value" } });
            {
                auto res = idx.find({ { "key", "value" } });
                ct::expect(res.size() == 1_i);
                ct::expect(res.front() == "test.txt"_sv);
            }

            idx.update("test.txt", { "key", "value2" });
            {
                auto res = idx.find({ { "key", "value" } });
                ct::expect(res.size() == 0_i);
            }

            {
                auto res = idx.find({ { "key", "value2" } });
                ct::expect(res.size() == 1_i);
                ct::expect(res.front() == "test.txt");
            }

            idx.update("not found", { "key", "value2" });
            idx.update("test.txt", { "not found", "value2" });
            {
                auto res = idx.find({ { "key", "value2" } });
                ct::expect(res.size() == 1_i);
                ct::expect(res.front() == "test.txt");
            }
            {
                auto res = idx.find({ { "key", "value" } });
                ct::expect(res.size() == 0_i);
            }
            idx.remove("test.txt");

        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            ct::expect(false);
        }
    };

    "list attributes"_test = [] {
        idx::index idx("test.db");

        idx.insert("test.txt",
                   {
                       { "k1", "v1" },
                       { "k2", "v2" },
                       { "k3", "v3" },
                       { "k4", "v4" },
                   });

        {
            auto attrs = idx.list_attributes("test.txt");
            ct::expect(attrs.size() == 4_i);
            ct::expect(attrs[0].key == "k1");
            ct::expect(attrs[0].value == "v1");
            ct::expect(attrs[1].key == "k2");
            ct::expect(attrs[1].value == "v2");
            ct::expect(attrs[2].key == "k3");
            ct::expect(attrs[2].value == "v3");
            ct::expect(attrs[3].key == "k4");
            ct::expect(attrs[3].value == "v4");
        }
        {
            auto attrs = idx.list_attributes("not found");
            ct::expect(attrs.size() == 0_i);
        }
    };

    "delete attribute"_test = [] {
        idx::index idx("test.db");

        try {
            idx.insert("test.txt", { { "key", "value" } });
            {
                auto res = idx.find({ { "key", "value" } });
                ct::expect(res.size() == 1_i);
                ct::expect(res.front() == "test.txt"_sv);
            }

            idx.remove_attributes("test.txt", "not found");
            {
                auto res = idx.find({ { "key", "value" } });
                ct::expect(res.size() == 1_i);
                ct::expect(res.front() == "test.txt"_sv);
            }

            idx.remove_attributes("test.txt", "key");
            {
                auto res = idx.find({ { "key", "value" } });
                ct::expect(res.size() == 0_i);
            }
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            ct::expect(false);
        }
    };
}};
