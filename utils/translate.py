from typing import List, Dict, Tuple

import subprocess


def get_drive_path() -> str:
    return '\\\\.\\PHYSICALDRIVE0'

def get_offset(drive_path: str) -> int:
    """
    Make a call to `mmls` to identify ESP partition offset  
    """
    o = subprocess.run(['mmls', drive_path], stdout=subprocess.PIPE)

    esp_line = [x for x in o.stdout.splitlines() if b'EFI system partition' in x]

    if not esp_line:
        raise ValueError('No ESP partition in mmls output')

    offset = int(esp_line[0].split()[2].decode().lstrip('0'))
    return offset

def get_all_inodes(drive_path: str, offset: int) -> List[Tuple[str, int]]:
    """
    Make a call to `fls` to retrieve tuples [filename, inode] of all files in the ESP
    """
    o = subprocess.run(['fls', '-o', str(offset), '-Fr', drive_path], stdout=subprocess.PIPE)

    tups = []
    for each in o.stdout.splitlines():
        if b'*' in each:
            continue
        inode, filename = each.split()[1:]
        inode  = int(inode.rstrip(b':').decode())
        filename = filename.decode()
        tups.append((filename, inode))
    return tups

def get_all_sectors(tups: List[Tuple[str, int]], drive_path: str, offset: int) \
    -> Dict[
        int, 
        Tuple[str, List[int]]
       ]:
    """
    Make a call to `istat` to retrieve inode -> sectors mapping
    """
    index = {}
    for each in tups:
        _, inode = each
        print('inode', inode)
        o = subprocess.run(['istat', '-o', str(offset), drive_path, str(inode)], stdout=subprocess.PIPE)
        idx = o.stdout.splitlines().index(b'Sectors:')

        sectors = []
        for line in o.stdout.splitlines()[idx+1:]:
            
            for sector in line.decode().split():
                index[int(sector)] = each
    print(set(list(index.values())))
    import json
    json.dump(index, open('index.json', 'w'), indent=4)
    return index


def main():
    drive_path = get_drive_path()
    esp_offset = get_offset(drive_path)
    print('esp_offset', esp_offset)
    inodes = get_all_inodes(drive_path, esp_offset)
    db = get_all_sectors(inodes, drive_path, esp_offset)


if __name__ == '__main__':
    main()

