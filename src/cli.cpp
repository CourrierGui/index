// SPDX-License-Identifier: GPL-3.0-or-later

#include <cli.hpp>

#include <iostream>
#include <cstring>

namespace idx {

    cmd parse_cmd(const char *name)
    {
        switch (name[0]) {
            case 'c':
                if (!std::strcmp(&name[1], "reate"))
                    return cmd::create_db;
                break;
            case 'd':
                if (!std::strcmp(&name[1], "el-attr"))
                    return cmd::del_attr;
                break;
            case 'g':
                if (!std::strcmp(&name[1], "et-attr"))
                    return cmd::get_attr;
                break;
            case 'l':
                if (std::strncmp(&name[1], "ist-", 4))
                    return cmd::unknown;

                switch (name[5]) {
                    case 'd':
                        if (!std::strcmp(&name[6], "b"))
                            return cmd::list_db;
                        break;
                    case 'f':
                        if (!std::strcmp(&name[6], "iles"))
                            return cmd::list_files;
                        break;
                }
                break;
            case 's':
                switch (name[2]) {
                    case 'l':
                        if (!std::strcmp(&name[1], "elect"))
                            return cmd::select_db;
                        break;
                    case 't':
                        if (!std::strcmp(&name[1], "et-attr"))
                            return cmd::set_attr;
                        break;
                }
                break;
            case '-':
                if (!std::strcmp(&name[1], "-help") || !std::strcmp(&name[1], "h"))
                    return cmd::help;
                break;
        }
        return cmd::unknown;
    }

} /* end of namespace idx */
