#pragma once

namespace idx {

    enum class cmd {
        unknown,
        set_attr,
        get_attr,
        del_attr,
        list_files,
        create_db,
        select_db,
        help,
        list_db,
    };

    cmd parse_cmd(const char *name);

} /* end of namespace idx */
