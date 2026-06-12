// tiene 12 bytes de sizeof
struct __attribute((__packed__)) simdata_t {

    // header
    uint8_t header1;
    uint8_t header2;

    // ejes
    uint8_t embrague;
    uint8_t acelerador;
    uint8_t freno;
    int16_t volante;

    // set 1
    uint8_t luzbaja  : 1;
    uint8_t giroizq  : 1;
    uint8_t giroder  : 1;
    uint8_t arranque : 1;
    uint8_t contacto : 1;
    uint8_t bocina   : 1;
    uint8_t cinturon : 1;
    uint8_t padding1 : 1;

    // set 2
    uint8_t luzalta    : 1;
    uint8_t parabrisa1 : 1;
    uint8_t parabrisa2 : 1;
    uint8_t padding2   : 5;

    // set 3
    uint8_t marcha5   : 1;
    uint8_t bocina2   : 1;
    uint8_t frenomano : 1;
    uint8_t marcha4   : 1;
    uint8_t marcha2   : 1;
    uint8_t reversa   : 1;
    uint8_t marcha1   : 1;
    uint8_t marcha3   : 1;

    // set 4
    uint8_t padding3     : 5;
    uint8_t marchasaltas : 1;
    uint8_t calentador   : 1;
    uint8_t padding4     : 1;

    // footer
    uint8_t checksum;
    uint8_t footer;
};

simdata_t getSimData();

