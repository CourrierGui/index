#pragma once

// SPDX-License-Identifier: GPL-3.0-or-later

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
        list_index,
    };

    cmd parse_cmd(const char *name);

} /* end of namespace idx */
