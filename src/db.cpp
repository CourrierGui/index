// SPDX-License-Identifier: GPL-3.0-or-later

#include <db.hpp>

#include <sstream>
#include <iostream>
#include <map>

#include <sqlite3.h>

namespace idx {

    static constexpr const char *create_db_command =
R"(
create table Files (
    file_name varchar(255),
    primary key (file_name)
);
create table FileIndex (
    key varchar(255),
    value varchar(255),
    file_name varchar(255),
    primary key (key, value, file_name)
);
)";

    void create_db(std::string db_name)
    {
        if (access(db_name.c_str(), F_OK) != -1) {
            std::cerr
                << "warn: '" << db_name << "' already exists\n";
            return;
        }

        SQLite::Database db{db_name, SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE};

        db.exec(create_db_command);
    }

    index::index(std::string dbname) :
        db_{dbname.c_str(), SQLite::OPEN_READWRITE}
    {
    }

    auto index::files() -> std::vector<std::string>
    {
        SQLite::Statement query(db_, "select * from Files");
        std::vector<std::string> res;

        while (query.executeStep())
            res.push_back(query.getColumn(0));

        return res;
    }

    void index::insert(std::string filename)
    {
        std::stringstream ss;

        ss << "insert into Files values ('"
           << filename
           << "');";
        try {
            db_.exec(ss.str());
        } catch (const SQLite::Exception& e) {
            // ignore duplicates, just don't insert them
            if (e.getErrorCode() != SQLITE_CONSTRAINT)
                throw e;
        }
    }

    void index::remove(std::string filename)
    {
        std::stringstream ss;

        ss << "delete from Files where file_name = '"
           << filename
           << "';";
        db_.exec(ss.str());

        ss.str("");
        ss << "delete from FileIndex where file_name = '" << filename << "'";
        db_.exec(ss.str());
    }

    void index::insert(std::string filename,
                       const std::vector<attribute>& attributes)
    {
        std::stringstream ss;

        insert(filename);

        for (const auto& attr: attributes) {
            ss.str("");
            ss << "insert into FileIndex values ('"
               << attr.key << "', '" << attr.value << "', '" << filename << "')";
            try {
                db_.exec(ss.str());
            } catch (const SQLite::Exception& e) {
                // ignore duplicates, just don't insert them
                if (e.getErrorCode() != SQLITE_CONSTRAINT)
                    throw e;
            }
        }
    }

    auto index::find(const std::vector<attribute>& attributes)
        -> std::vector<std::string>
    {
        std::map<std::string, size_t> values_found;
        std::vector<std::string> res;
        std::stringstream ss;

        if (attributes.size() == 0) {
            ss << "select file_name from Files";

            SQLite::Statement query(db_, ss.str());

            while (query.executeStep())
                res.push_back(query.getColumn(0));

            return res;
        }

        for (const auto& attr: attributes) {
            ss.str("");
            ss << "select file_name from FileIndex where ";
            ss << "key = '" << attr.key << "'";
            if (!attr.value.empty())
                ss << " and value = '" << attr.value << "'";

            SQLite::Statement query(db_, ss.str());

            while (query.executeStep()) {
                auto f = query.getColumn(0);
                if (values_found.contains(f))
                    values_found[f]++;
                else
                    values_found[f] = 1;
            }
        }

        for (auto s: values_found)
            if (s.second == attributes.size())
                res.push_back(s.first);

        return res;
    }

    void index::update(const std::string& filename, const attribute& attr)
    {
        std::stringstream ss;

        ss << "update FileIndex set value = '"
           << attr.value << "' where key = '"
           << attr.key << "' and file_name = '" << filename << "'";

            db_.exec(ss.str());
    }

    auto index::list_attributes(const std::string& filename)
        -> std::vector<attribute>
    {
        std::vector<attribute> res;
        std::stringstream ss;

        ss << "select key, value from FileIndex where file_name = '"
           << filename << "'";

        SQLite::Statement query(db_, ss.str());

        while (query.executeStep())
            res.push_back({ query.getColumn(0), query.getColumn(1) });

        return res;
    }

    // TODO remove several attributes
    void index::remove_attribute(const std::string& filename, const std::string& key)
    {
        std::stringstream ss;

        ss << "delete from FileIndex where key = '"
           << key << "' and file_name = '" << filename << "'";

        db_.exec(ss.str());
    }

} /* end of namespace idx */
