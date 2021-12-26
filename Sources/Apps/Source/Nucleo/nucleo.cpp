#include <QDebug>
#include <QtCore>

#include <iostream>
#include <unistd.h>
#include "main.h"

#include "nucleo.h"

#include "erpc_client_setup.h"
#include "erpc_nucleo_uart.h"

Nucleo::Nucleo(){}

void Nucleo::run() {
    qDebug() << "Starting eRPC Client ... !";

    /* Init eRPC client environment */
    /* UART transport layer initialization */
    erpc_transport_t transport = erpc_transport_serial_init("/dev/ttyACM0", 115200);

    /* MessageBufferFactory initialization */
    erpc_mbf_t message_buffer_factory = erpc_mbf_dynamic_init();

    /* eRPC client side initialization */
    erpc_client_init(transport, message_buffer_factory);


    uint8_t recData[256] = {0};
    binary_t *recStruct = new binary_t();
    recStruct->data = recData;
    recStruct->dataLength = 1;
    uint64_t push_cnt = 0;

    // int cnt = 0;
    bool status = false;
    while (1) {
        /* call eRPC functions */
        usleep(1000000);
        erpc_uart_HeartBeat(&status);
        qDebug() << "Hearth Beat Status : " << (status ? "true": "false");
        status = false;
        erpc_uart_RecvPushCount(&push_cnt, &status);
        qDebug() << "Push count : " << push_cnt << " - " << "status : " << (status ? "true": "false");
        status = false;
    }
}