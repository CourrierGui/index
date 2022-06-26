#include <dirent.h>

// SPDX-License-Identifier: GPL-3.0-or-later

#include <db.hpp>
#include <cli.hpp>

#include <cctype>
#include <iostream>
#include <fstream>

struct realpath_error {
    const char *filename;
    int err;
};

class safe_realpath {
    public:
        safe_realpath(const char *filename)
            : realpath_(realpath(filename, NULL))
        {
            if (!realpath_)
                throw realpath_error{filename, errno};
        }

        operator const char *() const
        {
            return realpath_;
        }

        ~safe_realpath()
        {
            free((void *)realpath_);
        }

    const char *realpath_;
};

void usage(const char *progname)
{
    std::cout
        << "Usage: " << progname << "<cmd>\n"
        << '\n'
        << "<cmd>:\n"
        << '\n'
        << "    set-attr:   <file> k1 v1 k2 v2...\n"
        << "                add the attributes k1 and k2 to <file> "
           "with values v1 and v2 respectively\n"
        << "    get-attr:   <f1> <f2> ...\n"
        << "                list the attributes of <f1>, <f2>, ...\n"
        << "    del-attr:   <f1> <k1> <k2> ...\n"
        << "                delete the attributes <k1>, <k2>, ... from <file>\n"
        << "    list-files: <index>\n"
        << "                list the files contained in <index>\n"
        << "    list-index:\n"
        << "                list available indexes\n"
        << "    create:     <index>\n"
        << "                create a new index named <index>\n"
        << "    select:     <index>\n"
        << "                set the current index to <index>\n";
}

// TODO add unit tests for this
// TODO create a parser for the command line and the help

std::string get_data_dir()
{
    std::string home = getenv("HOME");

    if (home.empty()) {
        std::cerr << "Environment variable 'HOME' not defined ??\n";
        exit(1);
    }
    // FIXME use XDG stuff
    // TODO use env var to change database and config dir
    return home + "/.local/share/index/";
}

std::string get_conf_dir()
{
    std::string home = getenv("HOME");

    if (home.empty()) {
        std::cerr << "Environment variable 'HOME' not defined ??\n";
        exit(1);
    }
    return home + "/.config/index/";
}

void select_db(const char *db_name)
{
    std::fstream f;

    f.open(get_conf_dir() + "current_index", std::ios::out);

    f << db_name;
}

std::string read_db_name()
{
    std::fstream f;
    std::string s;

    f.open(get_conf_dir() + "current_index");

    if (!f.is_open()) {
        std::cerr << "warn: no current database setup, use 'index select <dbname>'\n";
        return "";
    }

    // FIXME safe check s
    f >> s;
    if (s.empty()) {
        std::cerr << "warn: no current database setup, use 'index select <dbname>'\n";
        return "";
    }
    return s;
}

std::string build_full_db_name(std::string db_name = "")
{
    if (db_name.empty()) {
        db_name = read_db_name();
        if (db_name.empty()){
            std::cerr
                << "error: no index provided and no current index setup, "
                   "use 'index setup <index>'\n";
            exit(1);
        }

    }
    std::string full_name{get_data_dir()};

    full_name += db_name;
    full_name += ".db";

    return full_name;
}

void list_attributes(int argc, char **argv)
{
    idx::index idx(build_full_db_name());

    for (int i = 0; i < argc; i++) {
        auto path = safe_realpath{argv[i]};

        auto attributes = idx.list_attributes((const char *)path);

        for (auto attr: attributes)
            std::cout << path << ": " << attr.key << ", " << attr.value << '\n';
    }
}

void set_attributes(const char *filename, int argc, char **argv)
{
    if (argc % 2 != 0)
        return;

    auto path = safe_realpath(filename);
    std::vector<idx::attribute> values;

    for (int i = 0; i < argc; i += 2)
        values.push_back({argv[i], argv[i + 1]});

    idx::index idx(build_full_db_name());
    idx.insert((const char *)path, values);
}

void del_attributes(const char *filename, int argc, char **argv)
{
    idx::index idx(build_full_db_name());
    auto path = safe_realpath(filename);

    for (int i = 0; i < argc; i++)
        idx.remove_attribute((const char *)path, argv[i]);
}

void list_files(const char *db_name)
{
    idx::index idx(build_full_db_name(db_name));

    for (auto f: idx.files())
        std::cout << f << '\n';
}

void create_db(const char *filename)
{
    const char *iter = filename;
    while (*iter) {
        if (!std::isalnum(*iter++)) {
            std::cerr << "error: invalid database name '" << filename << "'";
            return;
        }
    }

    std::string path{get_data_dir()};

    path += filename;

    if (!path.ends_with(".db"))
        path += ".db";

    idx::create_db(path);
}

void list_index()
{
    auto dir = get_data_dir();
    DIR *d = opendir(dir.c_str());
    struct dirent *dirent;

    if (d) {
        std::string current_index = read_db_name() + ".db";

        while ((dirent = readdir(d)) != NULL) {
            std::string file{dirent->d_name};

            if (file == "." || file == "..")
                continue;

            // FIXME safe check file type and content
            if (file.ends_with(".db"))
                std::cout << file.substr(0, file.size() - 3)
                    << (file == current_index ? " (X)" : "")
                    << '\n';
        }
        closedir(d);
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
        return 1;

    try {

        switch (idx::parse_cmd(argv[1])) {
            case idx::cmd::del_attr:
                if (argc >= 3)
                    del_attributes(argv[2], argc - 3, argv + 3);
                break;
            case idx::cmd::get_attr:
                list_attributes(argc - 2, argv + 2);
                break;
            case idx::cmd::set_attr:
                if (argc >= 3)
                    set_attributes(argv[2], argc - 3, argv + 3);
                break;
            case idx::cmd::list_files:
                if (argc >= 3)
                    list_files(argv[2]);
                break;
            case idx::cmd::create_db:
                if (argc >= 3)
                    create_db(argv[2]);
                break;
            case idx::cmd::select_db:
                if (argc >= 3)
                    select_db(argv[2]);
                break;
            case idx::cmd::list_index:
                list_index();
                break;
            case idx::cmd::help:
                usage(argv[0]);
                return 0;
            case idx::cmd::unknown:
                std::cerr
                    << "Unknown command '" << argv[1]
                    << "', try '" << argv[0] << " --help'\n";
                return 1;
        }

    } catch (const SQLite::Exception& e) {
        std::cerr << "error: " << e.getErrorStr() << '\n';
        return e.getErrorCode();
    } catch (const realpath_error& e) {
        std::cerr
            << "error: failed to get realpath of '"
            << e.filename << "': " << strerror(e.err) << '\n';
        return e.err;
    }

    return 0;
}
