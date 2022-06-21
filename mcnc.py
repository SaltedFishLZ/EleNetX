
import os

PD91_BASE_DIR = os.path.join("mcnc", "pd91", "bench")
PD91_YALs = {
    # block
    "block" : {
        "ami33", "ami49", "apte", "hp", "xerox",
    },
    # floorplan
    "floorplan" : {
        "fan",
    },
    # mixed
    "mixed" : {
        "a3", "g2", "t3",
    },
}

def pd91_yal2json(base_dir, bench):
    pass
