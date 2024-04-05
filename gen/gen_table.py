import re

input_filename = "DerivedCoreProperties.txt"

lines = open(input_filename, "r").readlines()

re1 = re.compile("^ *([0-9A-F]+) *; *(\w+)")
re2 = re.compile("^ *([0-9A-F]+)\.\.([0-9A-F]+) *; *(\w+)")

xid_props = ["XID_Start", "XID_Continue"]

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
            id_from = single_id.group(1)
            id_to = single_id.group(2)
            prop = single_id.group(3)
            pass
        else:
            continue

    print(line)
