#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>


#include "kyk_utils.h"
#include "gens_block.h"
#include "block_store.h"
#include "kyk_wallet.h"
#include "kyk_file.h"
#include "kyk_block.h"
#include "kyk_validate.h"
#include "kyk_utxo.h"
#include "kyk_address.h"
#include "kyk_protocol.h"
#include "kyk_block.h"
#include "kyk_tx.h"
#include "dbg.h"

#define WALLET_NAME ".alice_wallet"
#define WALLET_AGENT "/AliceWallet:0.0.0.1/"

#define CMD_INIT           "init"
#define CMD_DELETE         "delete"
#define CMD_ADD_ADDRESS    "addAddress"
#define CMD_QUERY_BLOCK    "queryBlock"
#define CMD_QUERY_BALANCE  "queryBalance"
#define CMD_SHOW_ADDR_LIST "showAddrList"
#define CMD_MK_TX          "makeTx"
#define CMD_PING           "ping"
#define CMD_REQ_VERSION    "req-version"
#define CMD_REQ_GETHEADERS "req-getheaders"
#define CMD_REQ_GETDATA    "req-getdata"

#define DEFAULT_NODE "localhost"
#define DEFAULT_SERVICE "8333"

static int match_cmd(char *src, char *cmd);
static int cmd_add_address(struct kyk_wallet* wallet, const char* desc);
static int cmd_query_balance(const struct kyk_wallet* wallet);
static int cmd_show_addr_list(const struct kyk_wallet* wallet);
static int cmd_make_tx(struct kyk_wallet* wallet,
		long double btc_num,
		const char* btc_addr);

static int cmd_ping(const char *node, const char* service);
static int cmd_req_version(const char* node, const char* service);
static int cmd_req_getheaders(const char* node, const char* service, struct kyk_wallet* wallet);
static int cmd_req_getdata(const char* node, const char* service, struct kyk_wallet* wallet);

static void dump_tx_to_file(const struct kyk_tx* tx, const char* filepath);
static int be_rejected(const ptl_message* rep_msg);


int main(int argc, char *argv[])
{
    struct kyk_wallet* wallet = NULL;
    char *hmdir = NULL;
    char *wdir = NULL;
    char *errptr = NULL;
    int res = -1;

    hmdir = kyk_gethomedir();
    check(hmdir != NULL, "failed to find the current dir");
    wdir = kyk_pth_concat(hmdir, WALLET_NAME);
    check(wdir != NULL, "failed to find the wallet dir");

    if(argc == 1){
	printf("usage: %s command [args]\n", argv[0]);
	printf("init a wallet:      %s %s\n", argv[0], CMD_INIT);
	printf("make tx:            %s %s [amount] [address]\n", argv[0], CMD_MK_TX);
	printf("query blance:       %s %s\n", argv[0], CMD_QUERY_BALANCE);
	printf("show address list:  %s %s\n", argv[0], CMD_SHOW_ADDR_LIST);
	printf("add address:        %s %s [address label]\n", argv[0], CMD_ADD_ADDRESS);
	printf("delete wallet:      %s %s\n", argv[0], CMD_DELETE);
	printf("ping request:       %s %s\n", argv[0], CMD_PING);
	printf("version request:    %s %s\n", argv[0], CMD_REQ_VERSION);
	printf("getheaders request: %s %s\n", argv[0], CMD_REQ_GETHEADERS);
	printf("getdata request: %s %s\n", argv[0], CMD_REQ_GETDATA);
    }
    
    if(argc == 2){
	if(match_cmd(argv[1], CMD_INIT)){
	    if(kyk_file_exists(wdir)){
		printf("wallet is already in %s\n", wdir);
		return 0;
	    }
	    res = kyk_setup_spv_wallet(&wallet, wdir);
	    check(res == 0, "Failed to init wallet: kyk_setup_spv_wallet failed");
	    check(wallet, "Failed to init wallet: kyk_setup_spv_wallet failed");
	} else if(match_cmd(argv[1], CMD_DELETE)){
	    printf("please use system command `rm -rf %s` to delete wallet\n", wdir);
	} else if(match_cmd(argv[1], CMD_QUERY_BALANCE)){
	    wallet = kyk_open_wallet(wdir);
	    cmd_query_balance(wallet);
	} else if(match_cmd(argv[1], CMD_SHOW_ADDR_LIST)){
	    wallet = kyk_open_wallet(wdir);
	    cmd_show_addr_list(wallet);
	} else if(match_cmd(argv[1], CMD_PING)){
	    cmd_ping("localhost", "8333");
	} else if(match_cmd(argv[1], CMD_REQ_VERSION)){
	    cmd_req_version("localhost", "8333");
	} else if(match_cmd(argv[1], CMD_REQ_GETHEADERS)){
	    wallet = kyk_open_wallet(wdir);
	    check(wallet, "failed to open wallet");
	    cmd_req_getheaders("localhost", "8333", wallet);
	} else if(match_cmd(argv[1], CMD_REQ_GETDATA)){
	    wallet = kyk_open_wallet(wdir);
	    cmd_req_getdata("localhost", "8333", wallet);
	} else {
	    printf("invalid options\n");
	}
    }

    if(argc == 3){
	if(match_cmd(argv[1], CMD_QUERY_BLOCK)){
	    struct kyk_bkey_val* bval = NULL;
	    wallet = kyk_open_wallet(wdir);
	    check(wallet != NULL, "failed to open wallet");
	    bval = w_get_bval(wallet, argv[2], &errptr);
	    check(errptr == NULL, "failed to getblock %s", errptr);
	    if(bval == NULL){
		printf("No block record found\n");
	    } else {
		kyk_print_bval(bval);
		kyk_free_bval(bval);
	    }
	} else if(match_cmd(argv[1], CMD_ADD_ADDRESS)){
	    wallet = kyk_open_wallet(wdir);
	    check(wallet, "failed to open wallet");
	    res = kyk_wallet_check_config(wallet, wdir);
	    check(res == 0, "failed to kyk_wallet_check_config");
	    cmd_add_address(wallet, argv[2]);
	} else {
	    printf("invalid command %s\n", argv[1]);
	}
    }

    if(argc == 4){
	if(match_cmd(argv[1], CMD_MK_TX)){
	    long double btc_num = 0;
	    char* btc_addr = NULL;

	    btc_num = strtold(argv[2], NULL);
	    btc_addr = argv[3];
	    
	    wallet = kyk_open_wallet(wdir);
	    res = cmd_make_tx(wallet, btc_num, btc_addr);
	} else if(match_cmd(argv[1], CMD_PING)){
	    char* node = argv[2];
	    char* service = argv[3];
	    cmd_ping(node, service);
	} else {
	    printf("invalid command %s\n", argv[1]);
	}
    }

    if(wdir) free(wdir);
    if(wallet) kyk_destroy_wallet(wallet);
    
    return 0;

error:
    if(wdir) free(wdir);
    if(wallet) kyk_destroy_wallet(wallet);
    return -1;
}

int match_cmd(char *src, char *cmd)
{
    int res = 0;
    
    res = strcasecmp(src, cmd) == 0 ? 1 : 0;

    return res;
}

int cmd_add_address(struct kyk_wallet* wallet, const char* desc)
{
    int res = -1;
    
    check(wallet, "wallet can not be NULL");
    check(desc, "address desc can not be NULL");
    
    res = kyk_wallet_add_address(wallet, desc);
    check(res == 0, "Failed to kyk_wallet_add_address");

    return 0;

error:

    exit(1);
}

int cmd_query_balance(const struct kyk_wallet* wallet)
{
    uint64_t value = 0;
    long double balance = 0.0;
    int res = -1;

    check(wallet, "Failed to cmd_query_balance: wallet is NULL");

    res = kyk_wallet_query_total_balance(wallet, &value);
    check(res == 0, "Failed to cmd_query_balance: kyk_wallet_query_total_balance failed");

    balance = value / (double)ONE_BTC_COIN_VALUE;

    printf("%Lf BTC\n", balance);


    return 0;
    
error:

    return -1;
}

int cmd_show_addr_list(const struct kyk_wallet* wallet)
{
    char** addr_list = NULL;
    size_t len = 0;
    size_t i = 0;
    int res = -1;

    check(wallet, "Failed to cmd_show_addr_list: wallet is NULL");

    res = kyk_wallet_load_addr_list(wallet, &addr_list, &len);
    check(res == 0, "Failed to cmd_show_addr_list: kyk_wallet_load_addr_list failed");
    
    for(i = 0; i < len; i++){
	printf("%s\n", addr_list[i]);
    }

    return 0;

error:

    return -1;
}

int cmd_ping(const char* node, const char* service)
{
    ptl_message* rep_msg = NULL;
    int res = -1;

    res = kyk_ptl_ping_req(node, service, &rep_msg);
    check(res == 0, "Failed to cmd_ping");

    kyk_print_ptl_message(rep_msg);

    return 0;

error:

    return -1;
}

int cmd_req_version(const char* node, const char* service)
{
    ptl_message* req_msg = NULL;
    ptl_message* rep_msg = NULL;
    ptl_ver_entity* ver = NULL;
    ptl_ver_entity* rep_ver = NULL;
    ptl_payload* pld = NULL;
    int32_t vers = 70014;
    const char* ip_src = LOCAL_IP_SRC;
    int port = 0;
    uint64_t nonce = 0;
    const char* agent = WALLET_AGENT;
    int32_t start_height = 0;
    int res = -1;

    port = strtol(service, NULL, 10);

    res = kyk_build_new_version_entity(&ver, vers, ip_src, port, nonce, agent, strlen(agent), start_height);
    check(res == 0, "Failed to cmd_req_version: kyk_build_new_version_entity failed");

    res = kyk_new_seri_ver_entity_to_pld(ver, &pld);
    check(res == 0, "Failed to cmd_req_version");

    res = kyk_build_new_ptl_message(&req_msg, KYK_MSG_TYPE_VERSION, NT_MAGIC_MAIN, pld);
    check(res == 0, "Failed to cmd_req_version: kyk_build_new_ptl_message failed");

    res = kyk_send_ptl_msg(node, service, req_msg, &rep_msg);

    res = kyk_deseri_new_version_entity(&rep_ver, rep_msg -> pld -> data, NULL);

    printf("got message from server:\n");
    kyk_print_ptl_version_entity(rep_ver);

    return 0;

error:

    return -1;
}

int cmd_req_getheaders(const char* node, const char* service, struct kyk_wallet* wallet)
{
    ptl_gethder_entity* et = NULL;
    struct kyk_blk_hd_chain* rep_hd_chain = NULL;
    struct kyk_blk_hd_chain* wallet_hd_chain = NULL;
    struct kyk_blk_hd_chain* hd_chain = NULL;
    uint32_t version = 1;
    ptl_payload* pld = NULL;
    ptl_message* req_msg = NULL;
    ptl_message* rep_msg = NULL;
    size_t hd_inx = 0;
    int res = -1;

    res = kyk_load_blk_header_chain(&wallet_hd_chain, wallet);
    check(res == 0, "Failed to kyk_load_blk_header_chain");

    res = kyk_build_new_getheaders_entity(&et, version);
    check(res == 0, "kyk_build_new_getheaders_entity failed");

    res = kyk_new_seri_gethder_entity_to_pld(et, &pld);
    check(res == 0, "kyk_new_seri_gethder_entity_to_pld failed");

    res = kyk_build_new_ptl_message(&req_msg, KYK_MSG_TYPE_GETHEADERS, NT_MAGIC_MAIN, pld);
    check(res == 0, "kyk_build_new_ptl_message failed");

    res = kyk_send_ptl_msg(node, service, req_msg, &rep_msg);
    check(res == 0, "kyk_send_ptl_msg failed");

    printf("===============> RECEIVED RESPONSE FROM NODE:\n");
    kyk_print_ptl_message(rep_msg);

    res = kyk_deseri_headers_msg_to_new_hd_chain(rep_msg, &rep_hd_chain);
    check(res == 0, "Failed to cmd_req_getheaders: kyk_deseri_headers_msg_to_new_hd_chain failed");

    printf("===============> RECEIVED BLOCK HEADERS FROM NODE:\n");
    kyk_print_blk_hd_chain(rep_hd_chain);

    res = kyk_compare_hd_chain(wallet_hd_chain, rep_hd_chain, &hd_inx);
    check(res == 0, "Failed to cmd_req_getheaders: kyk_compare_hd_chain failed");

    if(hd_inx < rep_hd_chain -> len){
	/* have got some new headers needed to be saved to the block headers file */
	hd_chain = calloc(1, sizeof(*hd_chain));
	check(hd_chain, "Failed to cmd_req_getheaders: calloc failed");

	hd_chain -> len = rep_hd_chain -> len - hd_inx;
	hd_chain -> hd_list = rep_hd_chain -> hd_list + hd_inx;

	res = kyk_save_blk_header_chain(wallet, hd_chain, "ab");
	check(res == 0, "Failed to cmd_req_getheaders: kyk_save_blk_header_chain failed");
    }
    
    return 0;
    
error:

    return -1;
}

int cmd_req_getdata(const char* node, const char* service, struct kyk_wallet* wallet)
{
    struct kyk_blk_hd_chain* wallet_hd_chain = NULL;
    struct kyk_block_list* blk_list = NULL;
    struct kyk_block* blk;
    struct ptl_inv* inv_list = NULL;
    ptl_payload* pld = NULL;
    ptl_message* req_msg = NULL;
    ptl_message* rep_msg = NULL;
    varint_t inv_count = 0;
    varint_t i = 0;
    int sfd = 0;
    int res = -1;

    res = kyk_load_blk_header_chain(&wallet_hd_chain, wallet);
    check(res == 0, "Failed to kyk_load_blk_header_chain");

    res = kyk_hd_chain_to_inv_list(wallet_hd_chain, PTL_INV_MSG_BLOCK, &inv_list, &inv_count);
    check(res == 0, "Failed to cmd_req_getdata: kyk_hd_chain_to_inv_list failed");

    kyk_print_inv_list(inv_list, inv_count);

    blk_list = calloc(1, sizeof(*blk_list));
    check(blk_list, "Failed to cmd_req_getdata: calloc failed");

    blk_list -> len = inv_count;
    blk_list -> data = calloc(blk_list -> len, sizeof(*blk_list -> data));
    check(blk_list -> data, "Failed to cmd_req_getdata: calloc failed");

    res = kyk_seri_ptl_inv_list_to_new_pld(&pld, inv_list, inv_count);
    check(res == 0, "Failed to cmd_req_getdata: kyk_seri_ptl_inv_list_to_new_pld failed");

    res = kyk_build_new_ptl_message(&req_msg, KYK_MSG_TYPE_GETDATA, NT_MAGIC_MAIN, pld);
    check(res == 0, "kyk_build_new_ptl_message failed");

    res = kyk_socket_connect(node, service, &sfd);
    check(res == 0, "Failed to kyk_socket_connect");

    res = kyk_write_ptl_msg(sfd, req_msg);

    while(1){
	res = kyk_recv_ptl_msg(sfd, &rep_msg, KYK_PL_BUF_SIZE, NULL);
	check(res == 0, "Failed to cmd_req_getdata: kyk_recv_ptl_msg failed");	
	kyk_print_ptl_message(rep_msg);
	blk = blk_list -> data + i;
	res = kyk_deseri_block_from_blk_message(blk, rep_msg, NULL);
	if(be_rejected(rep_msg)){
	    ptl_reject_entity* et = NULL;
	    pld = rep_msg -> pld;
	    res = kyk_deseri_new_reject_entity(pld -> data, pld -> len, &et, NULL);
	    printf("rejected by node\n");
	    kyk_print_ptl_reject_entity(et);
	    break;
	}

	i++;
	if(i >= blk_list -> len){
	    break;
	}
    }

    kyk_print_kyk_block_list(blk_list);

    res = kyk_wallet_update_utxo_chain_with_block_list(wallet, blk_list);
    check(res == 0, "Failed to cmd_req_getdata: kyk_wallet_update_utxo_chain_with_block_list failed");
    
    /* kyk_free_kyk_block_list(blk_list); */

    close(sfd);

    return 0;

error:
    /* if(blk_list) kyk_free_kyk_block_list(blk_list); */
    if(sfd) close(sfd);
    
    return -1;
    
}

int cmd_make_tx(struct kyk_wallet* wallet,
		long double btc_num,
		const char* btc_addr)
{
    struct kyk_tx* tx = NULL;
    ptl_payload* pld = NULL;
    ptl_message* req_msg = NULL;
    /* ptl_message* rep_msg = NULL; */
    int sfd = 0;
    int res = -1;

    check(wallet, "Failed to cmd_make_tx: wallet is NULL");

    res = kyk_validate_address(btc_addr, strlen(btc_addr));
    if(res == -1){
	printf("invalid address\n");
	return -1;
    }

    res = kyk_spv_wallet_make_tx(&tx, wallet, btc_num, btc_addr);
    check(res == 0, "Failed to cmd_make_tx: kyk_wallet_make_tx failed");

    dump_tx_to_file(tx, "tmp/cmd_make_tx_dump.dat");

    res = kyk_seri_tx_to_new_pld(&pld, tx);
    check(res == 0, "Failed to cmd_make_tx: kyk_seri_tx_to_new_pld failed");

    res = kyk_build_new_ptl_message(&req_msg, KYK_MSG_TYPE_TX, NT_MAGIC_MAIN, pld);
    check(res == 0, "kyk_build_new_ptl_message failed");

    res = kyk_socket_connect(DEFAULT_NODE, DEFAULT_SERVICE, &sfd);
    check(res == 0, "Failed to kyk_socket_connect");

    res = kyk_write_ptl_msg(sfd, req_msg);

    /* while(1){ */
    /* 	res = kyk_recv_ptl_msg(sfd, &rep_msg, KYK_PL_BUF_SIZE, NULL); */
    /* 	check(res == 0, "Failed to cmd_make_tx: kyk_recv_ptl_msg failed");	 */
    /* 	kyk_print_ptl_message(rep_msg); */
    /* } */

    return 0;
    
error:

    return -1;
}

int be_rejected(const ptl_message* rep_msg)
{
    int res = 0;

    res = strcmp(rep_msg -> cmd, KYK_MSG_TYPE_REJECT) == 0 ? 1 : 0;

    return res;
}

void dump_tx_to_file(const struct kyk_tx* tx, const char* filepath)
{
    uint8_t buf[1000];
    size_t check_size = 0;
    FILE* fp = NULL;

    check_size = kyk_seri_tx(buf, tx);
    fp = fopen(filepath, "wb");
    fwrite(buf, sizeof(*buf), check_size, fp);

    fclose(fp);
}

