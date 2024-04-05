# based on https://github.com/unicode-rs/unicode-xid/blob/master/scripts/unicode.py
#
# Copyright 2011-2015 The Rust Project Developers. See the COPYRIGHT
# file at the top-level directory of this distribution and at
# http://rust-lang.org/COPYRIGHT.
#
# Licensed under the Apache License, Version 2.0 <LICENSE-APACHE or
# http://www.apache.org/licenses/LICENSE-2.0> or the MIT license
# <LICENSE-MIT or http://opensource.org/licenses/MIT>, at your
# option. This file may not be copied, modified, or distributed
# except according to those terms.
#
# Modification by Light Transport Entertainment Inc.
#
import re

input_filename = "DerivedCoreProperties.txt"

lines = open(input_filename, "r").readlines()


def group_cat(cat):
    cat_out = []
    letters = sorted(set(cat))
    cur_start = letters.pop(0)
    cur_end = cur_start
    for letter in letters:
        assert letter > cur_end, \
            "cur_end: %s, letter: %s" % (hex(cur_end), hex(letter))
        if letter == cur_end + 1:
            cur_end = letter
        else:
            cat_out.append((cur_start, cur_end))
            cur_start = cur_end = letter
    cat_out.append((cur_start, cur_end))
    return cat_out

def ungroup_cat(cat):
    cat_out = []
    for (lo, hi) in cat:
        while lo <= hi:
            cat_out.append(lo)
            lo += 1
    return cat_out

re1 = re.compile("^ *([0-9A-F]+) *; *(\w+)")
re2 = re.compile("^ *([0-9A-F]+)\.\.([0-9A-F]+) *; *(\w+)")

xid_props = ["XID_Start", "XID_Continue"]
props = {}

for line in lines:
    prop = ""
    id_from = 0
    id_to = 0

    single_id = re1.match(line)
    if single_id:
        id_from = single_id.group(1)
        id_to = id_from
        prop = single_id.group(2)
    else:
        range_ids = re2.match(line)
        if range_ids:
            id_from = range_ids.group(1)
            id_to = range_ids.group(2)
            prop = range_ids.group(3)
            pass
        else:
            continue

    if prop not in xid_props:
        continue

    if prop not in props:
        props[prop] = []

    props[prop].append((int(id_from, 16), int(id_to, 16)))


# optimize
for prop in props:
    props[prop] = group_cat(ungroup_cat(props[prop]))

s = ""
for key in props.keys():
    prop = props[key]
    s += "std::vector<std::pair<int, int>> k" + key + "Table = {\n"

    cnt = 0
    for pair in prop:
        if (cnt > 0):
            s += ",\n"
        s += "  {" + "{:#x}".format(pair[0]) + ", " + "{:#x}".format(pair[1]) + "}"
        cnt += 1

    s += "\n};\n"
    
with open("unicode-xid-table.inc", "w") as f:
    f.write(s)
