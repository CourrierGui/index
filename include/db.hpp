#pragma once

#include <string>
#include <vector>
#include <SQLiteCpp/SQLiteCpp.h>

namespace idx {

    struct attribute {
        std::string key;
        std::string value;
    };

    // FIXME remove useless string copy
    class index {
        public:
            index(std::string index_name);

            auto files() -> std::vector<std::string>;

            void insert(std::string filename);
            void insert(std::string filename,
                        const std::initializer_list<attribute>& attributes);

            auto find(const std::initializer_list<attribute>& attributes)
                -> std::vector<std::string>;

            void update(const std::string& filename, const attribute& attr);

            void remove(std::string filename);

            auto list_attributes(const std::string& filename)
                -> std::vector<attribute>;

            void remove_attributes(const std::string& filename, const std::string& key);

        private:
            SQLite::Database db_;
    };

} /* end of namespace idx */
