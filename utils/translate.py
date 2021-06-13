from typing import List, Dict, Tuple


def get_drive_path():
    return ''

def get_offset(drive_path: str):
    """
    Make a call to `mmls` to identify ESP partition offset  
    """
    pass

def get_all_inodes(drive_path: str, offset: int) -> List[Tuple[str, int]]:
    """
    Make a call to `fls` to retrieve tuples [filename, inode] of all files in the ESP
    """
    pass

def get_all_sectors(inodes: List[Tuple[str, int]], offset: int) \
    -> Dict[
        int, 
        Tuple[str, List[int]]
       ]:
    """
    Make a call to `istat` to retrieve inode -> sectors mapping
    """
    pass

def main():
    drive_path = get_drive_path()
    esp_offset = get_offset(drive_path)
    inodes = get_all_inodes(drive_path, offset)
    db = get_all_sectors(inodes, offset)


if __name__ == '__main__':
    main()

