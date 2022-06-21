/**
 * @file ya2json.c
 * @author Zheng liang (zhliang@berkeley.edu)
 * @brief translate YAL to JSON
 * @version 0.1
 * @date 2022-06-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "cJSON/cJSON.h"
#include "benchtypes.h"

#define false 0
#define true  1
#define ModuleInitiaID 1

char ModuleTypeStrings[6][MAXNAMESIZE] = {
    "ERROR", "STANDARD", "PAD", "GENERAL", "PARENT", "FEEDTHROUGH"
};

char* ModuleType2String(ModuleType ModType) {
    return ModuleTypeStrings[ModType];
}

cJSON* CoordPair2JSON(CoordPair* xypair) {
    cJSON* json = NULL;
    if (xypair != CoordPairNull) {
        json = cJSON_CreateObject();
        if (!cJSON_AddNumberToObject(json, "X", xypair->X))
            goto end;
        if (!cJSON_AddNumberToObject(json, "Y", xypair->Y))
            goto end;
    }
end:
    return json;
}

char* yal2json(Module *module) {
    char *string = NULL;

    cJSON *json = cJSON_CreateObject();

    /* add module name to JSON */
    if (!cJSON_AddStringToObject(json, "Name", module->Name))
        goto end;

    /* add module type to JSON */
    if (!cJSON_AddStringToObject(json, "ModType",
                                 ModuleType2String(module->ModType)))
        goto end;

    /* add dimensions to JSON */
dimensions:
    if (module->Dimensions) {
        cJSON *dimensions = cJSON_AddArrayToObject(json, "Dimensions");

        /* iterate over all coordinate pairs */
        int CoordPairNumber = 0;
        CoordPair *CurrCoordPair = module->Dimensions;
        while (CurrCoordPair != CoordPairNull) {
            /* append coordinate pair */
            cJSON* pair = CoordPair2JSON(CurrCoordPair);
            if (!pair) goto end;
            if (!cJSON_AddItemToArray(dimensions, pair)) goto end;

            /* move to the next node */
            CoordPairNumber++;
            CurrCoordPair = CurrCoordPair->Link;
        }

        if (CoordPairNumber != 4) {
            fprintf(stderr, "CoordPairNumber != 4\n");
        }
    }
    
    /* add io list (if exists) to JSON */
    /* TODO: not implemented now */
iolist:

    /* add network (if exists) to JSON*/
network:
    NetworkList *CurrNetworkList = module->NetworkListHead;
    if (CurrNetworkList) {
        cJSON *netlists = cJSON_AddArrayToObject(json, "Network");

        /* iterate over connections instance by instance */
        int ConnectedInstanceNum = 0;
        while (CurrNetworkList) {
            /* create a single netlist of the current instance */
            cJSON *netlist = cJSON_CreateObject();
            
            /* add instance name and corresponding module name */
            if (!cJSON_AddStringToObject(netlist, "instancename",
                                         CurrNetworkList->InstanceName))
                goto end;
            if (!cJSON_AddStringToObject(netlist, "modulename",
                                         CurrNetworkList->ModuleName))
                goto end;

            /* iterate over and append all signal names connected */
            SignalList *CurrSignal = CurrNetworkList->SignalListHead;
            if (!CurrSignal) goto end;
            cJSON *signalnames = cJSON_AddArrayToObject(netlist, "signalnames");
            while (CurrSignal) {
                /* create JSON string object and append to array */ 
                cJSON* signalname = cJSON_CreateString(CurrSignal->SignalName);
                if (!cJSON_AddItemToArray(signalnames, signalname)) goto end;
                /* move to next signal */
                CurrSignal = CurrSignal->Link;
            }

            /* append the current instance netlist to array */
            if (!cJSON_AddItemToArray(netlists, netlist)) goto end;
            
            /* move to the next instance netlist */
            ConnectedInstanceNum++;
            CurrNetworkList = CurrNetworkList->Link;
        }
    }

    /* print JSON to string */
    string = cJSON_Print(json);

end:
    if (string == NULL)
        fprintf(stderr, "Failed to print monitor.\n");
    cJSON_Delete(json);
    return string;
}



int main(int argc, char const *argv[])
{
    if (argc < 3) {
        printf("Usage: yal2json <input>.yal <output>.json!\n");
        return -1;
    }

    /* Open input and output files */
    FILE *InputFile, *OutputFile;
    InputFile = fopen(argv[1], "r");
    OutputFile = fopen(argv[2], "w");
    if (InputFile == NULL) {
        printf("Cannot open file %s\n", argv[1]);
        return -1;
    }
    if (OutputFile == NULL) {
        printf("Cannot open file %s\n", argv[2]);
        return -1;
    }

    /* JSON list wrapper */
    fprintf(OutputFile, "[\n");

    /*  Read modules */
    short Done = false;
    int ModuleNumber = ModuleInitiaID;
    for (ModuleNumber = ModuleInitiaID; !Done; ModuleNumber++) {
        Module module;
        GetModule(InputFile,
                  module.Name, &module.ModType,
                  &module.Dimensions, &module.IOListHead,
                  &module.NetworkListHead,
                  &module.PlacementListHead,
                  &module.CriticalNetListHead);

        if (module.ModType == ENDFILE)
            Done = true;
        else {
            /* translate the module to JSON */
            char* json_str = yal2json(&module);

            /* print JSON to output file */
            if (ModuleNumber > ModuleInitiaID)
                fprintf(OutputFile, ",\n");
            fprintf(OutputFile, "%s\n\n", json_str);
        }
    }
    /* Note: there is a ENDFILE module read by GetModule */
    printf("%d modules parsed\n", ModuleNumber - ModuleInitiaID - 1);


    /* JSON list wrapper */
    fprintf(OutputFile, "]\n");


    /* close input and output files */
    fclose(InputFile); fclose(OutputFile);

    return 0;
}
