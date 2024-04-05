# based on https://github.com/unicode-rs/unicode-xid/blob/master/scripts/unicode.py
#
# TODO: Optimize table.
import re

input_filename = "DerivedCoreProperties.txt"

lines = open(input_filename, "r").readlines()

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

    props[prop].append((id_from, id_to))


for prop in props:
    print(prop)
