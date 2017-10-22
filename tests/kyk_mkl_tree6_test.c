#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "kyk_utils.h"
#include "kyk_mkl_tree.h"

/* 数据来源于 https://webbtc.com/block/000000000000000002374f6983212149a67c394acae72926c88db2cf21db48bf.json */

/* mrkl_tree: [ */
/* "d29ebaace8d1c5f1d667cfccd94cc6e8e6f9c1c8ea53e57ce6fd3486dfe221dc", */
/* "03385d564538904c29eea80fbc16d52dea50ceffa0e48de36bd6f76951d05321", */
/* "ebfe653aa19f44b05b8f9d335acc5728a5ff736f68c817b8e0592b8b6dbb43f2", */
/* "c58d610d82be446eaf2c22267d5f8f39188e4bfc24b687fdccd2b3d5ae5f0c6c", */
/* "e986e1b158ac0162af88c84246bd5dcfdec4db81a26d8efc16725db32055c7f5", */
/* "d1db0abda250cca3dc361d551c9e198c7f2bcd4e40894bfab75aa68682c5f2d9", */
/* "ba4be61319ca53c4211e7361a44bdb9eba10ce3ee14e4cd4ea1929028ff7eff0", */
/* "30167a3530ff3c902bbb28c20bae4e19439092cf55b52a097a863677013cc330", */
/* "bc5f8eb760d675a73841967351273db770b19dd5e15260d7387c978b00df2a31", */
/* "7bb7b913aa3a9edfab8dbc512be641e06b9ff0f640ce951222c39380c231c813", */
/* "8a5590d95f06adb6f1b6d06db91d4aefa49639a3481675f7fd9fb7f8b0d44398", */
/* "61839c14b1da5be6544fa8c0ea615d7ddccbbd832047c49ebaa0e0b044377d12" */
/* ] */


void build_tx_buf_from_hex(struct kyk_tx_buf *tx_buf, const char *hexstr);

int main()
{
    char *tx1_hex = "01000000010000000000000000000000000000000000000000000000000000000000000000ffffffff2c0301c5061f4d696e656420627920416e74506f6f6c20736331302009b26e92205853573b30070000126f1400ffffffff0110e1964a000000001976a914d190c364c43ff16cbeedbb28308efef13df61f8188ac00000000";
    char *tx2_hex = "0100000006e09b6e4220086700674e612033215d0100eb6a518f7c51132631bf06c744bf72000000006a47304402206659abb61f9d95056b0d6d4d4b92011548f0f339401c7f1deae2316ce568c04602205f3a4ec130725d9802d3f11a37bc64ce39981fe7c795503f5ea2a2a95aaab747012103bd654314f4aed2a5f4beb491fc61b9425a53639ff78939694e2868ee99636243feffffff32ff926b08ae8ead198e74bf6be875e2833fbbdce8c802b579d1e7b08b1ae3d6000000006a47304402202be9ba6d2779e81ababea2999ce8d3c30947b735f6a1f1c1ccc613d899ecf45d02207a18f28cd8787241c52f44bdea4ae3605db3b0649bb35d8cb2314e5d9fea6673012102acdea0c698aeb572551359e710ef89aca3f67cbdc214417071c4c5a89a212272feffffff12e0add4effab78ee206c850fc95da06e5bd9fbfa3a4d17314fa75f734779e51000000006b4830450221009c972d8805b915d6550dd4c8965fecfacc806b854802925421851442b60ca3bb0220796b9ff263e994837b41bc9696e064fe0ede4fe8f0e80c000cc6ca18b4ae0846012103ece9865d1ef66ef575b1b2fd3a9009bfa644892d84f65e431b27ce5e9f34e4a8feffffff60b7741f619ead8bd51a9f9b448103f3846981b430fd271f605ec6767d27626a010000006b483045022100d3095641633a7fbcf12c8fc12b921539a1d38dc13757d6de1cdc48139a5a2b5302205a041efb051c76f47468d734867f8c638d65e2a5d27165edf2db195b5d4b14c301210220bb87e1384844d3b9073ca149eef5733ac1337464e5c6226d4e1507451e195efeffffffaa7a55f16633a804d6561e5df00ddfbce83209991d4773783c6f80e165910d97010000006a47304402207b893acdae27cb81aa1a19fa586d07d6e8efc036617377d95c558c4a870a479c0220197f5146630815c997a3ad3e687d8eab4b23075fb02e503b0f705ca8d1036f73012102b973e9173439c41a79794cf7bf93688a46c1911c1fd021c0df03886c026e6a78feffffff662258f635414e8699d89affb55f600bc1bd9a7ba68a0c38aea4d1fc23d5fd34010000006b483045022100d54db4de33c5d92d15ba4f366b9e4e3d8ec10d57f793abc206cac0b8380b43250220054d32abf44177abc31cc520f2324d26212220e472fd610617d111157689c5e701210261810a9460ac0d7713fce78439ba2ea875fd8dcf590444837586b303a3b411ccfeffffff02cc857600000000001976a914d0cba37ce340e085e693b0af0cc0b136a8977be488acac860f00000000001976a9145917e083260417452cdefd21aca79db091a2c18b88ac00c50600";
    char *tx3_hex = "0100000001407b9e8d6c46080c1c17fdab5b7507a0afd58a96d57a8004217a5e30a66e7fe1010000006a47304402207677f5dbcb4a573d63a6ed0c8de8cec28079b8ef97368f8c2d535bdeb4e4411302207fd15603078b2d58a7be9863d45d0be09822497d543464321267ff75c6e91afa012103c1544c5cb436deee91a39438f9f67cc2738595405a26e475d560e6c87d62dca7ffffffff0228a85200000000001976a91427b922d525a702971a37e04877650bb480a05a3088ac1c0b9200000000001976a914f87caeda2ccb41f69b685a5476701fc293884e2788ac00000000";
    char *tx4_hex = "010000000121b1a5b3eaa72e724fb3fbaa25e17da0e7332c6057b404dbf344dfc0d1854c48010000006a47304402202a08d1cee4b6350dbae5e5b0611678c960f3a27233b3a285b1e48a190a1a851b02206cc1fd23f63876552477a328643293c071ee1c061a8a1b68267e66396158bb3d012103e6920311b716e8c94436ae2e6e2f40f5ae7a78dd1ed6f0612d6f66a08d5b0dd1ffffffff02a0534800000000001976a91444ee4d72513f4998507195913188df6ca2cb80c088acc878b400000000001976a914cc5ea962de956de4bd195e2069412811c51c2e0a88ac00000000";
    char *tx5_hex = "0100000001f243bb6d8b2b59e0b817c8686f73ffa52857cc5a339d8f5bb0449fa13a65feeb010000006b483045022100e80abaa1ca02057d9e64698a5b836321a9c38e3fd30430d58e56a9f10eaf7c6b02206c68fed885b3640b0dcec1b17f4ac3042d1275f7ca36ea0a21dde21ee15065a30121031abe46f2ab0c43e84539896c5158ecf8d8c4ce492600a95468769b443cef5796ffffffff028ca93d00000000001976a9148b0e6679c7ab7493e3ca0c2070dc5afbeff1ea2f88ac28a85200000000001976a914d967ba784875e4ae98c3205a0925d07eceaea81f88ac00000000";
    char *tx6_hex = "01000000016775d382c0aa7298e5c85dd7d211f58e74f0c6417e0e7ce276d023b0fb41d5900b0000006a47304402204bec0334525b4772acd113a8eafbf2ef20a14ccdac304047ac67ece94fc2141b0220746291e7ce5bb690e969e0d6608c7a0a39fa46d74c9b0f0330f802033015a22a012103ba369c0510197e2f76ae2d2122f98e96b8271d72fa4e815f2bc49dbefeb7330bffffffff02819b8e05000000001976a91486814a802cc06a85cbdfcb72cde1b4a6c842c19788ace018d400000000001976a9149729aafcbd66550f314d15651445cefa114539a188ac00000000";
    
    struct kyk_tx_buf buf_list[6];
    struct kyk_mkltree_level *leaf_level;
    struct kyk_mkltree_level *root_level;

    build_tx_buf_from_hex(buf_list, tx1_hex);
    build_tx_buf_from_hex(buf_list+1, tx2_hex);
    build_tx_buf_from_hex(buf_list+2, tx3_hex);
    build_tx_buf_from_hex(buf_list+3, tx4_hex);
    build_tx_buf_from_hex(buf_list+4, tx5_hex);
    build_tx_buf_from_hex(buf_list+5, tx6_hex);
    leaf_level = create_mkl_leafs(buf_list, 6);
    root_level = create_mkl_tree(leaf_level);

    kyk_print_mkl_tree(root_level);
}


void build_tx_buf_from_hex(struct kyk_tx_buf *tx_buf, const char *hexstr)
{
    tx_buf -> bdy = kyk_alloc_hex(hexstr, &tx_buf -> len);    
}




    











