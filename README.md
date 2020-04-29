Open this folder in VSCode with PlatformIO extension installed.

Communication between NodeMCU and Arduino is setup using I2C.

NodeMCU should be master.

    SLOT_1,
    SLOT_2,
    SLOT_3,
    SLOT_4,
    SLOT_5,
    SLOT_6,
    SLOT_7,
    SLOT_8,
    SLOT_9,
    SLOT_10,
    SLOT_11,
    SLOT_12,
    SLOT_13,
    SLOT_14,
    SLOT_15,
    SLOT_16,
    SLOT_17,
    SLOT_18

Slot names are defined in above format.

Slot are sorted in acceding order and combined with '\*' between them and sent to arduino.
Maximum of 4 slots can be selected.

For example :

    SLOT_1*SLOT_6*SLOT_10*SLOT_16
