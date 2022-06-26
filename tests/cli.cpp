#include <clean-test/clean-test.h>

// SPDX-License-Identifier: GPL-3.0-or-later

#include <cli.hpp>

namespace ct = clean_test;
using namespace ct::literals;

auto const suite = ct::Suite{"Index", [] {
    "parse command"_test = [] {
        ct::expect(idx::parse_cmd("set-attr") == idx::cmd::set_attr);
        ct::expect(idx::parse_cmd("get-attr") == idx::cmd::get_attr);
        ct::expect(idx::parse_cmd("del-attr") == idx::cmd::del_attr);
        ct::expect(idx::parse_cmd("create") == idx::cmd::create_db);
        ct::expect(idx::parse_cmd("select") == idx::cmd::select_db);
        ct::expect(idx::parse_cmd("list-files") == idx::cmd::list_files);
        ct::expect(idx::parse_cmd("list-db") == idx::cmd::list_db);
        ct::expect(idx::parse_cmd("-h") == idx::cmd::help);
        ct::expect(idx::parse_cmd("--help") == idx::cmd::help);
        ct::expect(idx::parse_cmd("invalid") == idx::cmd::unknown);
    };
}};
