//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// © H2O.ai 2018
//------------------------------------------------------------------------------
#include <stdlib.h>  // abs
#include <string>  // memcpy
#include "datatable.h"
#include "utils.h"



/**
 * Load DataTable stored in NFF format on disk.
 *
 * colspec
 *     A DataTable object containing information about the columns of the
 *     datatable stored on disk. This object should contain 3 columns, with
 *     file names, stypes, and meta-information of each column in the stored
 *     datatable.
 *
 * nrows
 *     Number of rows in the stored datatable.
 */
DataTable* DataTable::load(DataTable* colspec, int64_t nrows, const std::string& path)
{
    int64_t ncols = colspec->nrows;
    Column** columns = nullptr;
    dtmalloc(columns, Column*, ncols + 1);
    columns[ncols] = nullptr;

    if (colspec->ncols != 3 && colspec->ncols != 5) {
        throw ValueError() << "colspec table should have had 3 or 5 columns, "
                           << "but " << colspec->ncols << " were passed";
    }
    SType stypef = colspec->columns[0]->stype();
    SType stypes = colspec->columns[1]->stype();
    if (stypef != ST_STRING_I4_VCHAR ||
        stypes != ST_STRING_I4_VCHAR) {
        throw ValueError() << "String columns are expected in colspec table, "
                           << "instead got " << stypef << " and "
                           << stypes;
    }

    StringColumn<int32_t>* colf =
        static_cast<StringColumn<int32_t>*>(colspec->columns[0]);
    StringColumn<int32_t>* cols =
        static_cast<StringColumn<int32_t>*>(colspec->columns[1]);

    const int32_t* offf = colf->offsets();
    const int32_t* offs = cols->offsets();

    std::string rootdir(path);
    if (!(rootdir.empty() || rootdir.back() == '/'))
      rootdir += "/";

    for (int64_t i = 0; i < ncols; ++i)
    {
        // Extract filename
        size_t fsta = static_cast<size_t>(abs(offf[i - 1]));
        size_t fend = static_cast<size_t>(abs(offf[i]));
        size_t flen = static_cast<size_t>(fend - fsta);

        std::string filename = rootdir;
        filename.append(colf->strdata() + fsta, flen);

        // Extract stype
        size_t ssta = static_cast<size_t>(abs(offs[i - 1]));
        size_t send = static_cast<size_t>(abs(offs[i]));
        size_t slen = static_cast<size_t>(send - ssta);
        if (!(slen == 3 || slen==2)) {
            throw ValueError() << "Incorrect stype's length: " << slen;
        }
        std::string stype_str(cols->strdata() + ssta, slen);
        SType stype = stype_from_string(stype_str);
        if (stype == ST_VOID) {
            throw ValueError() << "Unrecognized stype: " << stype_str;
        }

        // Load the column
        columns[i] = Column::open_mmap_column(stype, nrows, filename);
    }

    return new DataTable(columns);
}
