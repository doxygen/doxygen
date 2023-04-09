remove = ["names.txt", "remove.txt", "countries.txt", "cities.txt"]

banned = set()

for each in remove:
    with open(each, "r") as f:
        for each in f.readlines():
            banned.add(each)

file = open("test.txt", "w")
with open("wordlist.10000", "r") as f:
    for each in f.readlines():
        if each in banned:
            continue
        file.write(each)

file.close()
