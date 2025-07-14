import os

filecnt = 0
filename = f"file_{filecnt}.jpeg"
file_tmp = open(filename, "wb")

with open("raw_images", "rb") as txtfile:
    for line in txtfile:
        if line != b'**********\n':
            file_tmp.write(line)
        else:
            print("break")
            file_tmp.close()
            filecnt += 1
            filename = f"file_{filecnt}.jpeg"
            file_tmp = open(filename, "wb")

    file_tmp.close()

os.remove(filename)
