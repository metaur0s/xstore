
CURRENT_MODULE_ID = 0x10000

0bOOOXBBAA
  \\\ ||||____  8 OPCODES
      ||\\____      WHEN      | ORIG_SIZE | MODULE_ID | MODULE_NAME_LEN
      \\______      WHAT_SIZE | X_SIZE
           0bXX -> 0 A 4 -> 1 A 5

  0 0b00000000 00FWWWTT  ENTRY
                     TT  LEN 1-4 BASE 0 TIME
                  WWW    LEN 0-7 BASE 1 WHAT_SIZE
                 F       WITH/WITHOUT FILE
 63 0b00111111 00111111
 64 0b01000000 01CCCUUU  FILE
   --- O HASH LENGTH ENTAO E DE ACORDO COM O TAMANHO TOTAL
                    UUU  LEN 1-8 BASE 1 ORIG_SIZE
                 CCC     LEN 1-8 BASE 1 X_SIZE
127 0b01111111 01111111
128 0b10000000 10000PPP  OFFSET
                    PPP  LEN 1-8 BASE 0 OFFSET
135 0b10000111 10000111
136 0b10001000 10001000  RESERVED
191 0b10111111 10111111  END
192 0b11000000 11KKKKKK  CHECKSUM [1 - 64 BYTES] / [8 - 512 BITS]
                 KKKKKK  LEN 1-64 BASE ---
255 0b11111111 11111111

o checksum()
da o
u64 csum[8];

csum[0] = BE64(A);
csum[7] = BE64(H);

e ai da o memcpy() e memcmp() conforme o caso!

ao cksum() e passado o endereco onde escrever
   - o stream writer coloca na posicao atual e recua conforme o tamanho desejado
   - o reader segue o opcode e da o cmp conforme o tamanho
             o reader faz o cksum() em sua stack

--- nao usar csum dentro de ocntextos, pois ai nao pode recuperar arquivos perdidos!

// 'E UMA CONFIGURACAO, E NAO O PROTOCOLO!!!!
// SE EMITIR UM CHECKSUM, ESTE SERA O TIPO
//      RELATIVO A SIZE VS WIDTH - O QUE TAL CHECKSUM PODE PROTEGER
#define CHECKSUM_512_AT ( 16 * 1024 * 1024)
#define CHECKSUM_256_AT (  4 * 1024 * 1024)
#define CHECKSUM_128_AT (       512 * 1024)
#define CHECKSUM_64_AT  (              768)
#define CHECKSUM_32_AT  (              256)
#define CHECKSUM_16_AT  (               64)
#define CHECKSUM_8_AT   (                0)

// uma so funcaio checksum
// ele ira salvar o que for necessario conforme o size acima
u8  mask[]  masked    0 -  64
u64 words[k] BE64()  65 - 512


A BASE, PARA CADA WORD LENGTH
   ========        *     *   ====== NO CASO DO CSUM/POSITION, ESSA BASE AQUI É 0 E NÃO 1, POIS EXISTE SIM O 0
1               0x01  0x01      --->  POIS NAO EXISTE SIZE 0 - mas existe module_id 0 :S
2             0x0101  0x01 + 0x0100
3           0x010101  0x01 + 0x0100 + 0x010000
4         0x01010101  0x01 + 0x0100 + 0x010000 + 0x01000000
5       0x0101010101  0x01 + 0x0100 + 0x010000 + 0x01000000 + 0x0100000000
6     0x010101010101  0x01 + 0x0100 + 0x010000 + 0x01000000 + 0x0100000000 + 0x010000000000
7   0x01010101010101  0x01 + 0x0100 + 0x010000 + 0x01000000 + 0x0100000000 + 0x010000000000 + 0x01000000000000
8 0x0101010101010101  0x01 + 0x0100 + 0x010000 + 0x01000000 + 0x0100000000 + 0x010000000000 + 0x01000000000000 + 0x0100000000000000


0x0? ENTRY NULL
     ??   TIME
     ??   WHAT_SIZE
     U8   WHAT[WHAT_SIZE]
0x1? FILE   (BELONGS TO CURRENT MODULE HASH TREE)
     U64  ORIG_HASH[2]
     ??   ORIG_SIZE
     ??   X_SIZE  <----     É ESTE VALOR  QUE TEM QUE SER USADO PARA IR SALTANDO   /   SE != ORIG_SIZE, ENTAO ESTÁ COMPRESSED
     U8   X[X_SIZE]
0x2? MODULE CHANGE
     ??   MODULE_ID
0x3? MODULE CREATE
     ??   MODULE_NAME_LEN
     CHAR MODULE_NAME[MODULE_NAME_LEN]
0x5? POSITION
0x6? CHECKSUM
     ??  CSUM <---- DESDE CSUM_START
0x7F END  <-- AO FECHAR TEM QUE ANTES COLOCAR O CHECKSUM SE POS != CSUM_START

struct ctx_s {
 // O PYTHON LE DAQUI USANDO O INT.FROM_BYTES()
    u64 orig_hash[2];
    u64 when;
    u64 size; // what | orig | module_name
    u64 x_size;
    u64 pos; // what | x | module_name
    u64 reserved[2];
// -----
    int fd;
    uint cur_mod;
    u64 csum;
    void* csum_start;
    void* pos;
    void* end;
    char fpath[512];
};

match c_function_read_at_opcode(self.cctx):
   case ENTRY_NULL:
       when
       what_size
       what_pos
   case ENTRY_FILE:
       orig_hash[2]
       when
       what_size
       what_pos
   case FILE:
       orig_hash[2]
       orig_size
       x_size
       x_pos
       assert 1 <= x_size <= orig_size
    # NORMALMENTE SÓ ESTAMOS REGSTRANDO AS COISAS NO NOSSO DICT
    # ISSO AQUI É SÓ SE QUISERMOS LER O CONTEUDO
       if x_size != orig_size:
           x = decompress(x, orig_size)
   case MODULE_CHANGE:
        module_id
     assert module_id < len(mods)
     mod_current = module_id
   case MODULE_CREATE:
        module_name_len
        module_name_pos
     assert ((mod_current < len(mods)) or
             (mod_current == 0x10000 and len(mods) == 0))
     mod_current = len(mods)
     modNames.append(str.decode(buff[module_name_pos:module_name_pos+module_name_len]))
   case END:
        pass
   case _:
        raise ValueError
