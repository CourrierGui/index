#pragma once

// SPDX-License-Identifier: GPL-3.0-or-later

#include <string>
#include <vector>
#include <SQLiteCpp/SQLiteCpp.h>

namespace idx {

    struct attribute {
        std::string key;
        std::string value;
    };

    void create_db(std::string db_name);

    // FIXME remove useless string copy
    class index {
        public:
            index(std::string index_name);

            auto files() -> std::vector<std::string>;

            void insert(std::string filename);
            void insert(std::string filename,
                        const std::vector<attribute>& attributes);

            auto find(const std::vector<attribute>& attributes)
                -> std::vector<std::string>;

            void update(const std::string& filename, const attribute& attr);

            void remove(std::string filename);

            auto list_attributes(const std::string& filename)
                -> std::vector<attribute>;

            void remove_attribute(const std::string& filename, const std::string& key);

        private:
            SQLite::Database db_;
    };

} /* end of namespace idx */
