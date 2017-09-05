/*
* Android.mk //TODO:Here is makefile reference
* LOCAL_PATH:= $(call my-dir)

* include $(CLEAR_VARS)
* LOCAL_SRC_FILES:= pn6xt_spi_test.c

* LOCAL_MODULE:= pn6xt_spi_test

* include $(BUILD_EXECUTABLE)
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

#define PN544_MAGIC 0xE9
/*
  NFC and SPI will call the ioctl to update the power scheme
*/
#define P544_SET_POWER_SCHEME _IOW(PN544_MAGIC, 0x07, long)
/*
 * SPI Request NFCC to enable p61 power, only in param
 * Only for SPI
 * level 1 = Enable power
 * level 0 = Disable power
 */
#define P61_SET_SPI_PWR    _IOW(PN544_MAGIC, 0x02, long)

/* SPI or DWP can call this ioctl to get the current
 * power state of P61
 *
*/
#define P61_GET_PWR_STATUS    _IOR(PN544_MAGIC, 0x03, long)

extern int teeclientdemo(int argc, char *argv[]);
unsigned char  core_reset_cmd[] = {0x20, 0x00, 0x01, 0x01};
unsigned char  core_init_cmd[] = {0x20, 0x01, 0x00};
unsigned char  nxp_property_cmd[] = {0x2F, 0x02, 0x00};
unsigned char  nfcee_discover_cmd[] = {0x22, 0x00, 0x01, 0x01};
unsigned char nfcee_mode_set_cmd[] = {0x22, 0x01, 0x02, 0x01, 0x01};
unsigned char nfcee_ese_mode_set_cmd[] = {0x22, 0x01, 0x02, 0xC0, 0x00};
unsigned char nfcee_core_conn_create_cmd[] = {0x20, 0x04, 0x06, 0x03, 0x01, 0x01, 0x02, 0x01, 0x01};
unsigned char core_conn_credits_ntf[] = {0x60, 0x06, 0x03, 0x01, 0x03, 0x01};

unsigned char hci_admin_open[] = {0x03, 0x00, 0x02, 0x81, 0x03};
unsigned char hci_admin_get_session_id[] = {0x03, 0x00, 0x03, 0x81, 0x02, 0x01};
unsigned char hci_admin_set_whitelist[] = {0x03, 0x00, 0x04, 0x81, 0x01, 0x03, 0xC0};
unsigned char hci_admin_get_host_list[] = {0x03, 0x00, 0x03, 0x81, 0x02, 0x04};

unsigned char ese_test_data[] = {0x03, 0x00, 0x07, 0x99, 0x50, 0x80, 0xCA, 0x9F, 0x7F, 0x00}; //get-cplc
unsigned char get_dwp_status_cmd[] = {0x20, 0x03, 0x05,0x02,0xA0,0xEC,0xA0,0xED};
unsigned char nxp_set_pwr_req_internal_cmd[] = {0x20, 0x02, 0x05,0x01,0xA0,0xF2,0x01,0x00};//Supplied ESE power internally 
unsigned char nxp_set_pwr_req_external_cmd[] = {0x20, 0x02, 0x05,0x01,0xA0,0xF2,0x01,0x01};//Supplied ESE power externally
unsigned char nxp_set_standby_cmd[] = {0x2F, 0x00, 0x01, 0x00};

unsigned char data1[] = {0x03, 0x00, 0x03, 0x99, 0x02, 0x01};
unsigned char data2[] = {0x03, 0x00, 0x02, 0x99, 0x01};

int send_nci_data(int fp, unsigned char data[], int len)
{
	int ret = 0;
	int i;
	int retry = 0;

	while (1)
	{
		printf("NxpNciX : len =  %-4d> ", data[2]+3);
		for (i=0; i<len; i++)
		{
			printf("%.2X", data[i]);
		}
		printf("\n");
		
		ret = write(fp, data, len);
		if (ret < 0)
		{
			printf("\npn544 write error, maybe in standby mode,  retcode = %d, errno = %d, retry...\n", ret, errno);
			//wait 50ms
			usleep(5000);
			retry++;
			
			if (retry > 3) break;
		}
		else
		{
			break;
		}
	}
	return ret;
}

int recv_nci_data(int fp, unsigned char recv_data[])
{
	int ret = 0;
	int i;
	//int read_data_again = *read_again;
	//Read pn547 responses, Read XX XX XX 00 ..., if 4th byte is 00, it means STATUS_OK.
	memset(recv_data, 0, sizeof(recv_data));
	ret = read(fp, &recv_data[0], 3);
	if (ret < 0) {
		printf("\npn544 read nci head error! retcode = %d, errno = %d", ret, errno);
		close(fp);	
		return errno;
	}
	else if (recv_data[0] == 0x51) {
		printf("\nRead 0x51, IRQ may do not work, abort! ");
		return ret = 0x51;
	}
	
	printf("NxpNciR : len =  %-4d> ", recv_data[2]+3);
	for (i=0; i<3; i++)
		printf("%.2X", recv_data[i]);
	
	ret = read(fp, &recv_data[3], recv_data[2]);
	if (ret < 0) 
		printf("\npn544 read nci payload error! retcode = %d, errno = %d", ret, errno);
		
	for (i=0; i<recv_data[2]; i++)
		printf("%.2X", recv_data[i+3]);
	
	printf("\n");	
	return ret;
}



int send_ese_data_by_spi(int fp, unsigned char data[], int len)
{
	int ret = 0;
	int i;
	
	printf("AP->eSE: ");
	for (i=0; i<len; i++)
	{
		printf("%.2X ", data[i]);
	}
	printf("\n");
	
	ret = write(fp, data, len);
	if (ret < 0){
		printf("\np61 write error, maybe in standby mode,  retcode = %d, errno = %d, retry...", ret, errno);
		//wait 50ms
		usleep(50000);
		ret = write(fp, data, len);
		if (ret < 0){
			printf("\np61 write error retco= %d, errno = %d(%s)", ret, errno, strerror(errno));
			return ret;
		}
	}
	return ret;
}

int recv_ese_data_by_spi(int fp, unsigned char recv_data[], int len)
{
	int ret = 0;
	int i;
	
	//memset(recv_data, 0, sizeof(recv_data));
	ret = read(fp, &recv_data[0], len);
	if (ret < 0) {
		printf("\np61 read spi data error! retcode = %d, errno = %d\n", ret, errno);
		close(fp);	
		return errno;
	}

	printf("eSE->AP: ");
	for (i=0; i<ret; i++)
		printf("%.2X ", recv_data[i]);
	
	printf("\n");	
	return ret;
}




/**  data_msg_exchange
*/
int data_msg_exchange(int fp, unsigned char data_msg[], int len)
{
	int ret;
	unsigned char recv_data[255];
	
//	printf("data_msg_exchange\n");

	ret = send_nci_data(fp, data_msg, len);
	memset(recv_data, 0, sizeof(recv_data));
	ret = recv_nci_data(fp, recv_data);
	ret = memcmp(recv_data, core_conn_credits_ntf, sizeof(core_conn_credits_ntf));
	
	if (ret == 0) //send_data-----recv_credit----recv_data, such as apdu transmit
	{
		//read recv data
		while(1)
		{
			memset(recv_data, 0, sizeof(recv_data));
			ret = recv_nci_data(fp, recv_data);
			if (recv_data[0] == 0x03) break;
		}
		
	}
	else //send_data----notification----recv_data----recv_credit, such as hci init
	{
		while(1)
		{
			ret = recv_nci_data(fp, recv_data);
			ret = memcmp(recv_data, core_conn_credits_ntf, sizeof(core_conn_credits_ntf));
			if (ret == 0) 
                            break;
		}
	}
	
	printf("\n");
	return ret;
}


int nci_reset_int(int fp)
{	int ret;
	int len;
	unsigned char recv_data[255];
	
	printf("nci_reset_init\n");
	ret = send_nci_data(fp, core_reset_cmd, sizeof(core_reset_cmd));
if(ret < 0)
{
  printf("nci_reset_init: send core_reset_cmd error \n");
  return ret;
}
	memset(recv_data, 0, sizeof(recv_data));
	ret = recv_nci_data(fp, recv_data);
	
	ret = send_nci_data(fp, core_init_cmd, sizeof(core_init_cmd	));
	memset(recv_data, 0, sizeof(recv_data));
	ret = recv_nci_data(fp, recv_data);
	
	printf("\n");
	return ret;
}


int nci_nxp_property(int fp)
{
	int ret;
        int len;
        unsigned char recv_data[255];

        printf("nci_nxp_property_cmd\n");
        ret = send_nci_data(fp, nxp_property_cmd, sizeof(nxp_property_cmd));
        memset(recv_data, 0, sizeof(recv_data));
        ret = recv_nci_data(fp, recv_data);

        printf("\n");
        return ret;

}

int nxp_set_standby(int fp)
{       int ret;
        int len;
        unsigned char recv_data[255];

        printf("nxp_set_standby\n");
        ret = send_nci_data(fp, nxp_set_standby_cmd, sizeof(nxp_set_standby_cmd));
        memset(recv_data, 0, sizeof(recv_data));
        ret = recv_nci_data(fp, recv_data);

        printf("\n");
        return ret;
}

int nci_get_dwp_swp_status(int fp)
{       int ret;
        int len;
        unsigned char recv_data[255];

        printf("nci_get_dwp_swp_status\n");
        ret = send_nci_data(fp, get_dwp_status_cmd, sizeof(get_dwp_status_cmd));
        memset(recv_data, 0, sizeof(recv_data));
        ret = recv_nci_data(fp, recv_data);

        printf("\n");
        return ret;
}

int nxp_set_pwr_req_internal(int fp)
{       int ret;
        int len;
        unsigned char recv_data[255];

        printf("nxp_set_pwr_req_cmd internal\n");
        ret = send_nci_data(fp, nxp_set_pwr_req_internal_cmd, sizeof(nxp_set_pwr_req_internal_cmd));
        memset(recv_data, 0, sizeof(recv_data));
        ret = recv_nci_data(fp, recv_data);

        printf("\n");
        return ret;
}

//nxp_set_pwr_req_external_cmd
int nxp_set_pwr_req_external(int fp)
{       int ret;
        int len;
        unsigned char recv_data[255];

        printf("nxp_set_pwr_req_cmd external\n");
        ret = send_nci_data(fp, nxp_set_pwr_req_external_cmd, sizeof(nxp_set_pwr_req_external_cmd));
        memset(recv_data, 0, sizeof(recv_data));
        ret = recv_nci_data(fp, recv_data);

        printf("\n");
        return ret;
}

int nfcee_discover(int fp)
{
	int ret;
      	int len;
	unsigned char recv_data[255];
	
	printf("nfcee_discover\n");
	ret = send_nci_data(fp, nfcee_discover_cmd, sizeof(core_reset_cmd));
	memset(recv_data, 0, sizeof(recv_data));
	ret = recv_nci_data(fp, recv_data);
	//read nfcee_discover_ntf
	memset(recv_data, 0, sizeof(recv_data));
	ret = recv_nci_data(fp, recv_data);
	
	printf("\n");
	return ret;
}

int nfcee_mode_set(int fp)
{
	int ret;
	int len;
	unsigned char recv_data[255];
	
	printf("nfcee_mode_set\n");
	ret = send_nci_data(fp, nfcee_mode_set_cmd, sizeof(nfcee_mode_set_cmd));
	memset(recv_data, 0, sizeof(recv_data));
	ret = recv_nci_data(fp, recv_data);
	
	//read nfcee_mode_set_ntf
	memset(recv_data, 0, sizeof(recv_data));
	ret = recv_nci_data(fp, recv_data);
	
	printf("\n");
	return ret;
}

int nfcee_core_conn_create(int fp)
{
	int ret;
	int len;
	unsigned char recv_data[255];
	
	printf("nfcee_core_conn_create\n");
	ret = send_nci_data(fp, nfcee_core_conn_create_cmd, sizeof(nfcee_core_conn_create_cmd));
	memset(recv_data, 0, sizeof(recv_data));
	ret = recv_nci_data(fp, recv_data);
	
	printf("\n");
	return ret;
}


/**
* this hci ese init function is not used as first boot mode
* the RF_NFCEE_DISCOVERY_REQ_NTF will activate the hci init between nfcc and ese automatically

int hci_ese_init(fp)
{
	int ret;
	int len;
	unsigned char recv_data[255];

	
	printf("hci_open\n");
	len = sizeof(hci_admin_open);
	ret  = data_msg_exchange(fp, hci_admin_open, len);

	printf("hci_get_session_id\n");
	len = sizeof(hci_admin_get_session_id);
	ret  = data_msg_exchange(fp, hci_admin_get_session_id, len);

	printf("hci_set_whitelist\n");
	len = sizeof(hci_admin_set_whitelist);
	ret  = data_msg_exchange(fp, hci_admin_set_whitelist, len);

	printf("hci_get_host_list\n");
	len = sizeof(hci_admin_get_host_list);
	ret  = data_msg_exchange(fp, hci_admin_get_host_list, len);

	printf("\n");
	return ret;
}

*/

int nfcee_ese_discovery_and_mode_set(int fp)
{
	int ret;
	printf("nfcee_ese_discovery_and_mode_set\n");
	nfcee_discover(fp);
	nfcee_core_conn_create(fp);
	nfcee_mode_set(fp);
	
	return ret;
}

int nfcee_ese_test(int fp)
{
	int len;
	int ret;
	
	printf("ese_test_data\n");
	len = sizeof(ese_test_data);
	ret = data_msg_exchange(fp, ese_test_data, len);
	
	return ret;
}

#define ESE_SOF  0xA5
static int read_cplc_data(int fp)
{
    unsigned char recv_data[256];
    int count = 0, total_count = 0;
    int ret = 0;
    int i = 0;

    memset(recv_data, 0, sizeof(recv_data));
    do
    {
    count++;
        ret = read(fp, recv_data, 1);
        printf("count:%d --0x%x ,ret:%d\n", count, recv_data[0], ret);

        if(0x00 == recv_data[0])
        {
            ret = read(fp, recv_data, 1);
            printf("count-2:%d --0x%x ,ret:%d\n", count, recv_data[0], ret);
        }

    }while(recv_data[0] != ESE_SOF && count < 8);

    if (recv_data[0] != ESE_SOF) // if the Last time is not the desise value
    {
        printf("spi_read SOF timeout\n");
        return  -ETIME;
    }  

    total_count = 1;
    ret = read(fp, &recv_data[1], 2);
total_count += 2;
printf("%s:dma_length= 0x%x, rx_buffer[2]=0x%x\n", __func__, recv_data[1], recv_data[2]);

   ret = read(fp, &recv_data[3], recv_data[2]+1);
   total_count = total_count + recv_data[2]+1;
printf("%s:total_count=%d\n", __func__, total_count);


        printf("eSE-->AP: CPLC Responce DATA\n");
//	ret = recv_ese_data_by_spi(esefp, recv_data, sizeof(recv_data));
//        printf("recv_ese_data_by_spi ret = %d \n", ret);

    	printf("Header: ");
	for (i=0; i<3; i++)
		printf("%.2X ", recv_data[i]);
    	printf("\n CPLC: ");
	for (; i<48; i++)
		printf("%.2X", recv_data[i]);
    	printf("\n Rsp Status: ");
	for (; i<51; i++)
		printf("%.2X", recv_data[i]);
       printf("\n\n");


    return ret;
}





static int ese_read_data(int fp)
{
    unsigned char recv_data[256];
    int count = 0, total_count = 0;
    int ret = 0;
    int i = 0;

    memset(recv_data, 0, sizeof(recv_data));
    do
    {
        count++;
        ret = read(fp, recv_data, 1);
        printf("count:%d --0x%x ,ret:%d\n", count, recv_data[0], ret);

//        if(0x00 == recv_data[0])
//        {
//            ret = read(fp, recv_data, 1);
//            printf("count-2:%d --0x%x ,ret:%d\n", count, recv_data[0], ret);
//        }

    }while(recv_data[0] != ESE_SOF && count < 1000);

    if (recv_data[0] != ESE_SOF) // if the Last time is not the desise value
    {
        printf("spi_read SOF timeout\n");
        return  -ETIME;
    }  

    total_count = 1;
    ret = read(fp, &recv_data[1], 2);
    total_count += 2;
    printf("%s:recv_data[1]= 0x%x, rx_buffer[2]=0x%x(below length)\n", __func__, recv_data[1], recv_data[2]);

   ret = read(fp, &recv_data[3], recv_data[2]+1);
   total_count = total_count + recv_data[2]+1;
    printf("%s:total_count=%d\n", __func__, total_count);


    printf("eSE->AP: ");
    for (i=0; i < total_count; i++)
        printf("%.2X ", recv_data[i]);
	
    printf("\n\n");	

    ret = total_count;
    return ret;
}


unsigned char reset_cmd[] = {0x5A, 0xC4, 0x00, 0xC4};
unsigned char open_ch_cmd[] = {0x5A, 0x00, 0x04, 0x01, 0x70, 0x80, 0x01, 0xF4};
      //5A400E00A4040008A0000001510000000016
unsigned char select_isd_cmd[] = {0x5A, 0x00, 0x0E, 0x01, 0xA4, 0x04, 0x00, 0x08, 0xA0, 0x00, 0x00, 0x01, 0x51, 0x00, 0x00, 0x00, 0x00, 0x57};
unsigned char cplc_cmd[] = {0x5A, 0x00 , 0x05 , 0x80 , 0xCA , 0x9F , 0x7F , 0x00 , 0xAF}; //{0x5A, 0x00, 0x05, 0x81, 0xCA, 0x9F, 0x7F, 0x00, 0xAE};
//5A000401708001F4 close ch?
unsigned char cmd_end[] = {0x5A, 0x40, 0x05, 0x00, 0xA4, 0x04, 0x00, 0x00, 0xE5};

//unsigned char cmd_end[] = {0x5A, 0x40, 0x05, 0x00, 0xA4, 0x04, 0x00, 0x00, 0xE5};

static int ese_reset_cmd(int fp)
{
    int readcount = 0;
    int ret = 0, i = 0;
    
    do
    {
        printf("AP-->eSE: reset_cmd CMD index:%d \n", i);
        ret = send_ese_data_by_spi(fp, reset_cmd, sizeof(reset_cmd)); 
        readcount = ese_read_data(fp);
        i++;
    }while( 0);

    return readcount;
}

static int ese_common_cmd_test(int fp, unsigned char *pcmd, int cmdlen)
{
    int readcount = 0;
    int ret = 0, i = 0;
    
    do
    {
        printf("AP-->eSE: reset_cmd CMD index:%d \n", i);
        ret = send_ese_data_by_spi(fp, pcmd, cmdlen); 
        readcount = ese_read_data(fp);
    }while( (readcount <= 4) &&  (i++ < 10));
  
    return readcount;
}


/*
//get CPLC with logic 1
5A00040170000174       // open logic 1
5A000E01A4040008A0000001510000000057
5A000581CA9F7F00AE  
5A000401708001F4      // close logic 1
*/

unsigned char open_logic_1[] = {0x5A, 0x00, 0x04, 0x01, 0x70, 0x00, 0x01, 0x74};
unsigned char select_isd[] = {0x5A, 0x00, 0x0E, 0x01, 0xA4, 0x04, 0x00, 0x08, 0xA0, 0x00, 0x00, 0x01, 0x51, 0x00, 0x00, 0x00, 0x00, 0x57};

unsigned char cplc_cmd_logic1[] = {0x5A, 0x00, 0x05, 0x81, 0xCA, 0x9F, 0x7F, 0x00, 0xAE};
unsigned char clsoe_logic_1[] = {0x5A, 0x00, 0x04, 0x01, 0x70, 0x80, 0x01, 0xF4};
static int cplc_test_80t_logicCh1(int fp)
{
    int ret =0;
#if 0
    printf("AP-->eSE: Send reset_cmd CMD\n");
    ret = send_ese_data_by_spi(fp, reset_cmd, sizeof(reset_cmd)); 
    ret = ese_read_data(fp);
#else
           ese_reset_cmd(fp);
#endif
/*
    printf("AP-->eSE: Send open_ch_cmd CMD\n");
    ret = send_ese_data_by_spi(fp, open_ch_cmd, sizeof(open_ch_cmd)); 
    ret = ese_read_data(fp);
*/
    printf("AP-->eSE: Send select_isd CMD\n");
    ret = send_ese_data_by_spi(fp, select_isd, sizeof(select_isd)); 
    ret = ese_read_data(fp);

    printf("AP-->eSE: Send cplc_cmd CMD\n");
    ret = send_ese_data_by_spi(fp, cplc_cmd_logic1, sizeof(cplc_cmd_logic1)); 
    ret = ese_read_data(fp);

   printf("AP-->eSE: CMD end\n");
   ret = send_ese_data_by_spi(fp, clsoe_logic_1, sizeof(clsoe_logic_1)); 
   ret = ese_read_data(fp);
    
    return 0;
}


// get CPLC with logic 0
//5A400E00A4040008A0000001510000000016 
// 5A000580CA9F7F00AF   

unsigned char select_isd_logic0[] = {0x5A, 0x40, 0x0E, 0x01, 0xA4, 0x04, 0x00, 0x08, 0xA0, 0x00, 0x00, 0x01, 0x51, 0x00, 0x00, 0x00, 0x00, 0x16};

unsigned char cplc_cmd_logic0[] = {0x5A, 0x00, 0x05, 0x80, 0xCA, 0x9F, 0x7F, 0x00, 0xAF};

static int cplc_test_80t_logicCh0(int fp)
{
    int ret =0;
#if 0
    printf("AP-->eSE: Send reset_cmd CMD\n");
    ret = send_ese_data_by_spi(fp, reset_cmd, sizeof(reset_cmd)); 
    ret = ese_read_data(fp);
#else

            ese_reset_cmd(fp);

#endif
/*
    printf("AP-->eSE: Send open_ch_cmd CMD\n");
    ret = send_ese_data_by_spi(fp, open_ch_cmd, sizeof(open_ch_cmd)); 
    ret = ese_read_data(fp);
*/
    printf("AP-->eSE: Send select_isd_cmd CMD\n");
    ret = send_ese_data_by_spi(fp, select_isd_logic0, sizeof(select_isd_logic0)); 
    ret = ese_read_data(fp);

    printf("AP-->eSE: Send cplc_cmd CMD\n");
    ret = send_ese_data_by_spi(fp, cplc_cmd_logic0, sizeof(cplc_cmd_logic0)); 
    ret = ese_read_data(fp);

 //   printf("AP-->eSE: CMD end\n");
 //   ret = send_ese_data_by_spi(fp, cmd_end, sizeof(cmd_end)); 
 //   ret = ese_read_data(fp);
    
    return 0;
}

static int cplc_test_80t_basiCh0(int fp)
{
    int ret =0;
#if 0
    printf("AP-->eSE: Send reset_cmd CMD\n");
    ret = send_ese_data_by_spi(fp, reset_cmd, sizeof(reset_cmd)); 
    ret = ese_read_data(fp);
#else

            ese_reset_cmd(fp);

#endif
/*
    printf("AP-->eSE: Send open_ch_cmd CMD\n");
    ret = send_ese_data_by_spi(fp, open_ch_cmd, sizeof(open_ch_cmd)); 
    ret = ese_read_data(fp);

    printf("AP-->eSE: Send select_isd_cmd CMD\n");
    ret = send_ese_data_by_spi(fp, select_isd_cmd, sizeof(select_isd_cmd)); 
    ret = ese_read_data(fp);
*/
    printf("AP-->eSE: Send cplc_cmd CMD\n");
    ret = send_ese_data_by_spi(fp, cplc_cmd, sizeof(cplc_cmd)); 
    ret = ese_read_data(fp);

 //   printf("AP-->eSE: CMD end\n");
 //   ret = send_ese_data_by_spi(fp, cmd_end, sizeof(cmd_end)); 
 //   ret = ese_read_data(fp);
    
    return 0;
}



int main(int argc, char *argv[])
{
    int ret = 0;
    int fp = 0, esefp = 0;
    int len;
    int i = 0;
    unsigned char* nci_cmd;
    unsigned char recv_data[128];

	unsigned char send_test[] = {0x5A, 0x00, 0x05, 0x00, 0x70, 0x00, 0x00, 0x01, 0x74};
    
 //    unsigned char ese_test_data[] = {0x00, 0x00 , 0x05 , 0x80 , 0xCA , 0x9F , 0x7F , 0x00 , 0xAF};//66T
 //5A000580CA9F7F00AF
   unsigned char ese_test_data[] = {0x5A, 0x00 , 0x05 , 0x80 , 0xCA , 0x9F , 0x7F , 0x00 , 0xAF};//80T
//	unsigned char ese_test_data[] = {0x80 , 0xCA , 0x9F , 0x7F , 0x00};
//	unsigned char ese_test_data[] = {0x80, 0xf2, 0x40, 0x00, 0x12, 0x4f, 0x10, 0xa0, 0x00, 0x00, 0x03, 0x33, 0x01, 0x01, 0x01, 0x00, 0x03, 0x05, 0x00, 0x00, 0x00, 0x11, 0x01, 0x00};

    //a000000151000000
    unsigned char test_aid1[] = {0x00, 0x00, 0x0D, 0x00, 0xA4, 0x04, 0x00, 0x08, 0xA0, 0x00, 0x00, 0x01, 0x51, 0x00, 0x00, 0x00, 0x55};
    //a000000003000000
    unsigned char test_aid2[] = {0x00, 0x00, 0x0D, 0x00, 0xA4, 0x04, 0x00, 0x08, 0xA0, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x06};

    //test CUP AID 'A0000003330053440121561126241000
    unsigned char test_CUP[] = {0x00, 0x00, 0x0D, 0x00, 0xA4, 0x04, 0x00, 0x08, 0xA0, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x06};

#if 1
    //ese p61 spi hardware reset
    printf("p553 spi driver testing... 0712\n");
    ret = (esefp = open("/dev/pn553", O_RDWR));
    if (ret < 0)
    {
        printf("pn553 driver node open error retcode = %d, errno = %d\n", ret, errno);
        exit(0);
    }
    printf("pn553 driver node open OK!\n");
          
    int ese_current_state = 0;
    ret = ioctl(esefp, P544_SET_POWER_SCHEME, 2);
    printf("P544_SET_POWER_SCHEME OK! ret=%d\n", ret);   


    ret =ioctl(esefp, P61_GET_PWR_STATUS, &ese_current_state);
    printf("p61 P61_GET_SPM_STATUS OK! ret=%d, ese_current_state=0x%x\n", ret, ese_current_state);   

    ret = ioctl(esefp, P61_SET_SPI_PWR, 3);
    printf("p61 P61_SET_SPM_PWR OK! ret=%d\n", ret);   

    //If you want NFC-S2c Can access ese, must set to 4, thit releae prio spi mode
    ret = ioctl(esefp, P61_SET_SPI_PWR, 4);
    printf("p61 P61_SET_SPM_PWR OK! ret=%d\n", ret);   
#endif
 sleep(1);

#if 1
      printf("call teeclientdemo\n");
      teeclientdemo(argc,argv); 
	  printf("return teeclientdemo\n");
#endif 
	ret = ioctl(esefp, P61_SET_SPI_PWR, 0);
    printf("p61 P61_SET_SPM_PWR 0 OK! ret=%d\n", ret);  
	close(esefp);

	return 0;
}
