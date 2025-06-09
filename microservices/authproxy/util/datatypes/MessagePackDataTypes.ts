import {decode, encode} from "@msgpack/msgpack";
import {RawData} from "ws";

export type ServerMessage = [
    RequestInfo,
    ServerMessageObjects
];

type RequestInfo = [
    number[],
    number[],
    number[],
    number[]
];

type ServerMessageObjects = [
    string,
    ObjectContent
];

type ObjectContent = [
    Record<string, DummyObject>,
    Planet[],
    Record<string, Player>
];

type Player = [
    string,
    number,
    CraftingMaterial
];

type CraftingMaterial = [
    number
];

type DummyObject = [
    string,
    number,
    string,
    Coordinate,
    Coordinate
];

type Planet = [
    string,
    Coordinate,
    BuildingRegion[],
    number
];

type Mine = [
    string,
    CraftingMaterial,
]

type Factory = [
    string,
    CraftingMaterial,
]

type BuildingRegion= [
    Coordinate,
    Factory[],
    Mine[],
    CraftingMaterial
]

type Coordinate = [
    number,
    number,
    number
];

type ClientData = [
    setClientFPS | null,
    string | null,
    dummySetVelocity | null,
    string | null
]

type dummySetVelocity = [
    number,
    Coordinate
]

type setClientFPS = [
    number,
]

export type ClientMessage = [
    RequestInfo,
    ClientData,
    string,
]

export async function printServerMessageStructure() {
    const serverMessage: ServerMessage = [
        [
            [0.0],
            [0.0],
            [0.0],
            [1749129250857.0]
        ],
        [
            "Server Response",
            [
                {
                    "0": ["jonas", 0, "name", [0.0, 0.0, 0.0], [0.0, 0.0, 0.0]]
                },
                [
                    [
                        "mercury",
                        [-0.3289997977941043, -0.2996735458190398, 0.006088886030626311],
                        [[
                            [0.7071067811865475, -0.7071067811865475, 0.0],
                            [],
                            [],
                            [0.0]
                        ]],
                        1.0
                    ],
                    [
                        "venus",
                        [0.6120664364827312, 0.38578690936213006, -0.030218669377590865],
                        [[
                            [0.7071067811865475, -0.7071067811865475, 0.0],
                            [],
                            [],
                            [0.0]
                        ]],
                        1.0
                    ],
                    [
                        "mars",
                        [-1.6504366590367459, -0.024743910270102896, 0.04019865757308089],
                        [[
                            [0.7071067811865475, -0.7071067811865475, 0.0],
                            [],
                            [],
                            [0.0]
                        ]],
                        1.0
                    ],
                    [
                        "jupiter",
                        [3.2897999666382356, -3.875956701997485, -0.0569138932088357],
                        [[
                            [0.7071067811865475, -0.7071067811865475, 0.0],
                            [],
                            [],
                            [0.0]
                        ]],
                        1.0
                    ],
                    [
                        "saturn",
                        [-0.5340364430614233, 9.008385517833242, -0.1376979188524779],
                        [[
                            [0.7071067811865475, -0.7071067811865475, 0.0],
                            [],
                            [],
                            [0.0]
                        ]],
                        1.0
                    ],
                    [
                        "uranus",
                        [-0.16944115528544174, 19.068229221579564, 0.07238799679238803],
                        [[
                            [0.7071067811865475, -0.7071067811865475, 0.0],
                            [],
                            [],
                            [0.0]
                        ]],
                        1.0
                    ],
                    [
                        "neptune",
                        [28.665175492504805, 8.353348299399046, -0.8285199849027337],
                        [[
                            [0.7071067811865475, -0.7071067811865475, 0.0],
                            [],
                            [],
                            [0.0]
                        ]],
                        1.0
                    ],
                    [
                        "earth",
                        [0.040093286792015924, 0.9831025043703968, 0.0],
                        [[
                            [0.7071067811865475, -0.7071067811865475, 0.0],
                            [],
                            [],
                            [0.0]
                        ]],
                        1.0
                    ]
                ],
                {
                    "jonas": ["jonas", 0.0, [451.0]]
                }
            ]
        ]
    ];
    console.log(String(encode(serverMessage)));
}

export async function printClientMessageStructure(){
    const clientMessage: ClientMessage = [
        [
            [
                0
            ],
            [
                0
            ],
            [
                0
            ],
            [
                0
            ]
        ],
        [
            null,
            null,
            [
                0,
                [
                    2,
                    0,
                    0
                ]
            ],
            null
        ],
        "jonas"
    ];
    console.log(String(encode(clientMessage)));
}

export async function decodeToServerMessage(msg: RawData) {
    const uint8Array = msg instanceof Buffer
        ? new Uint8Array(msg)
        : new Uint8Array(msg as ArrayBuffer);

    return decode(uint8Array) as ServerMessage;
}

export async function encodeServerMessage(msg: ServerMessage) {
    return encode(msg);
}

export async function encodeClientMessage(msg: ClientMessage){
    return encode(msg);
}

export async function decodeToClientMessage(msg: RawData) {
    const uint8Array = msg instanceof Buffer
        ? new Uint8Array(msg)
        : new Uint8Array(msg as ArrayBuffer);

    return decode(uint8Array) as ClientMessage;
}

