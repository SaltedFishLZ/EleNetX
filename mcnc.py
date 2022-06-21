
import os
import json
import subprocess


PD91_BASE_DIR = os.path.join("mcnc", "pd91", "bench")

PD91_BENCHMARKS = {
    # block design
    "block" : {
        "ami33" : {
            "format" : ["yal", "vpnr"],
            "technology" : "ami"
        },
        "ami49" : {
            "format" : ["yal", "vpnr"],
            "technology" : "ami"
        },
        "apte" : {
            "format" : ["yal", "vpnr"],
            # TODO: technology
        },
        "hp" : {
            "format" : ["yal", "vpnr"],
            "technology" : "ami"
        },
        "xerox" : {
            "format" : ["yal", "vpnr"],
            "technology" : "xerox"
        },
    },
    
    # floorplan
    "floorplan" : {
        "fan" : {
            "format" : ["yal", "vpnr"],
            # self-contained technology
        },
        # TODO: xerox
    },
    
    # mixed
    "mixed" : {
        "a3" : {
            "format" : ["yal", "vpnr"],
            # self-contained technology
        },
        "g2" : {
            "format" : ["yal", "vpnr"],
            # self-contained technology
        },
        "t1" : {
            "format" : ["yal", "vpnr"],
            # self-contained technology
        },
    },

    # std cell (benchmark only, no tech lib)
    "stdcell" : {
        "biomed" : {
            "format" : ["yal", "vpnr"],
            "technology" : "db"
        },
        "fract" : {
            "format" : ["yal", "vpnr"],
            "technology" : "db"
        },
        "industry1" : {
            "format" : ["yal", "vpnr"],
            # self-contained technology
        },
        "industry2" : {
            "format" : ["yal", "vpnr"],
            # self-contained technology
        },
        "industry3" : {
            "format" : ["yal", "vpnr"],
            # self-contained technology
        },
        # TODO: primary1 and primary2
        # need fix the YAL c parser to parse HEIGHT and WIDTH
        # "primary1" : {
        #     "format" : ["yal", "vpnr"],
        #     "technology" : "sclib"
        # },
        # "primary2" : {
        #     "format" : ["yal", "vpnr"],
        #     "technology" : "sclib"
        # },
        "struct" : {
            "format" : ["yal", "vpnr"],
            "technology" : "db"
        },
    }
}

PD91_TECHNOLOGIES = {
    "stdcell" : {
        "db" : {
            "format" : ["yal", "vpnr"],
        },
        "sclib" : {
            "format" : ["yal", "vpnr"],
        },
        # TODO: gate array
        # need fix the YAL c parser to parse HEIGHT and WIDTH
    }
}

def pd91_yal2json(base_dir):
    YAL2JSON_TRANSLATOR_PATH = os.path.join("yal", "c-parser", "yal2json")
    # translate benchmark YAL files
    for category in PD91_BENCHMARKS:
        for benchmark in PD91_BENCHMARKS[category]:
            if "yal" in PD91_BENCHMARKS[category][benchmark]["format"]:
                print("=" * 64)
                print("translating {} from yal to json".format(benchmark))
                print("=" * 64)
                src_yal_path = os.path.join(PD91_BASE_DIR, category,
                                            benchmark + ".yal")
                dst_json_path = os.path.join(PD91_BASE_DIR, category,
                                             benchmark + ".json")
                subprocess.run([
                    YAL2JSON_TRANSLATOR_PATH,
                    src_yal_path,
                    dst_json_path
                ])

if __name__ == "__main__":
    pd91_yal2json(base_dir=PD91_BASE_DIR)