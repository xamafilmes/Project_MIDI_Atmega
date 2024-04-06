#define F_CPU 16000000UL

#include <avr/interrupt.h>
#include "funsape/peripheral/twi.hpp"
#include "funsape/device/keypad.hpp"
#include "funsape/globalDefines.hpp"
#include "funsape/peripheral/usart0.hpp"
#include "funsape/peripheral/timer2.hpp"

#define issetBit(REG, bit)  ((REG)&(1<<bit))
#define setBit(reg, bit)                ((reg) |= (1 << (bit)))
#define clrBit(reg, bit)                ((reg) &= ~(1 << (bit)))
#define cplBit(reg, bit)                ((reg) ^= (1 << (bit)))
#define setMaskOffset(reg, mask, offset)        ((reg) |= ((mask) << (offset)))
#define MPU9250_SLAVEADRESS             0b1101000

//------------UCSRnA – USART Control and Status Register n A-----------------
#define RXCE0                7
//This flag bit is set when there are unread data in the receive buffer
//and cleared when the receive buffer is empty

#define TXC0                6
//This flag bit is set when the entire frame in the
// Transmit Shift Register has been shifted out and there are no
// new data currently present in the transmit buffer

#define UDRE0               5                               //**** see if 1
// The UDREn Flag indicates if the transmit buffer (UDRn) is
// ready to receive new data. If UDREn is one, the
// buffer is empty, and therefore ready to be written.

#define FE0                 4
// This bit is set if the next character in the receive buffer
// had a Frame Error when received. I.e., when the first stop
// bit of the next character in the receive buffer is zero

#define DOR0                3
// This bit is set if a Data OverRun condition is detected.
// A Data OverRun occurs when the receive buffer is full
// (two characters)

#define UPE0                2
// This bit is set if the next character in the receive buffer had
// a Parity Error when received and the Parity Checking
// was enabled at that point (UPMn1 = 1).

#define U2X0                1                               //**** 0
// This bit only has effect for the asynchronous operation.
// Write this bit to zero when using synchronous operation.
// Writing this bit to one will reduce the divisor of the baud rate
// divider from 16 to 8 effectively doubling the transfer
// rate for asynchronous communication.

#define MPCM0               0
// This bit enables the Multi-processor Communication mode.
// When the MPCMn bit is written to one, all the
// incoming frames received by the USART Receiver
// that do not contain address information will be ignored

//------------UCSRnA – USART Control and Status Register n A-----------------

//UDRn – USART I/O Data Register n

//------------UCSRnB – USART Control and Status Register n B-----------------

#define RXCIE0              7
// Writing this bit to one enables interrupt on the RXCn Flag.

#define TXCIE0             6
// Writing this bit to one enables interrupt on the TXCn Flag.

#define UDRIE0              5                               //**** 1
// Writing this bit to one enables interrupt on the UDREn Flag.

#define RXEN0              4
// Writing this bit to one enables the USART Receiver.

#define TXEN0               3                               //**** 1
// Writing this bit to one enables the USART Transmitter.
// The Transmitter will override normal port operation for the TxDn
// pin when enabled.
// The disabling of the Transmitter (writing TXENn to zero)

#define UCSZ02              2                               //**** 0
// The UCSZn2 bits combined with the UCSZn1:0 bit in UCSRnC sets the number
// of data bits

#define RXB80               1
// RXB8n is the ninth data bit of the received character when operating with
// serial frames with nine data bits

#define TXB80               0
// TXB8n is the ninth data bit in the character to be transmitted when
// operating with serial frames with nine data bits

//------------UCSRnB – USART Control and Status Register n B-----------------


//------------UCSRnC – USART Control and Status Register n C-----------------

#define UMSEL01             7                               //**** 0
// These bits select the mode of operation of the USARTn
// as shown in Table 20-8

#define UMSEL00             6                               //**** 0
// These bits select the mode of operation of the USARTn as shown in Table 20-8

#define UPM01               5                               //**** 0
// These bits enable and set type of parity generation and check

#define UPM00               4                               //**** 0
// These bits enable and set type of parity generation and check

#define  USBS0              3                               //**** 0
// This bit set the number of stop bits 0 = 1, 1 = 2.

#define  UCSZ01             2                               //**** 1
// The UCSZn1:0 bits combined with the UCSZn2 bit in UCSRnB
// sets the number of data bits

#define  UCSZ00             1                               //**** 1
// The UCSZn1:0 bits combined with the UCSZn2 bit in UCSRnB
// sets the number of data bits

#define UCPOL0              0                               //**** 0
// This bit is used for synchronous mode only. Write this
// bit to zero when asynchronous mode is used.

//------------UCSRnC – USART Control and Status Register n C-----------------


//------------ UBRRnL and UBRRnH – USART Baud Rate Registers-----------------

//Bit 15:12 – Reserved  these bit must be written to zero when UBRRnH is written.

// 0:10 - prescaller BaudRate.

//------------ UBRRnL and UBRRnH – USART Baud Rate Registers-----------------


typedef unsigned char                 uint8;
typedef char                          int8;

// "1XXX CCCC" is the MIDI channel (from 0 to 15)
// "0PPP PPPP" is the pitch value (from 0 to 127)
// "0VVV VVVV" is the velocidade_city value (from 0 to 127)

// The pitch value determines the frequency of the note to be played.
// It goes from 0 to 127, with the middle C note being represented by the
// value of 60: C# will be 61, D will be 62,...

// por default a função note_on (tocar nota) soma 60 ao valor da nota
#define C           0
#define C_s         1
#define D_b         1
#define D           2
#define D_s         3
#define E_b         3
#define E           4
#define F           5
#define F_s         6
#define G_b         6
#define G           7
#define G_s         8
#define A_b         8
#define A           9
#define A_s         10
#define B_b         10
#define B           11

// The instrument is according to General MIDI standart and goes form 0 to
// 127

// The velocidade_city goes from 1 to 127, covering the range from
// a practically inaudible note up to the maximum note level
// velocidade_city 0 is for Note_off
// pppp = 8; ppp = 20; pp = 31; p = 42; mp = 53; mf = 64; f = 80; ff = 96
// fff = 112; ffff = 127

#define pppp 8
#define ppp  20
#define pp   31
#define p    42
#define mp   53
#define mf   64
#define f    80
#define ff   96
#define fff  112
#define ffff 127

struct MIDI {
    uint8 STATUS_BYTE = 0b10000000; // primeiro byte a ser enviado
    uint8 DATA_BYTE1  = 0b00000000; // segundo byte a ser enviado
    uint8 DATA_BYTE2  = 0b00000000; // terceiro byte a ser enviado
    uint8 DATA_SENT   = 0b00000000; // bit 0 a 1 controla a quantidade
    // de envios, bit 2 e 3 o modo tocar nota ou mudar de instrumento
    // respectivamente.
    uint8 MIDI_CHANEL = 0b00000000; // seta o canal
};

typedef MIDI        Midi_t;


// -----------------------------config----------------------------------------------
// The TXCn Flag can be used to check that the
// Transmitter has completed all transfers
// Note that the TXCn Flag must be cleared
// before each transmission (before UDRn is
// written).

// The frame format used by the USART is set by the UCSZn2:0, UPMn1:0 and USBSn bits in UCSRnB and UCSRnC.
//The USART Character SiZe (UCSZn2:0) bits select the number of data bits in the frame.
// The USART Parity mode (UPMn1:0) bits enable and set the type of parity bit.
// The selection between one or two stop bits is done by the USART Stop Bit Select (USBSn) bit.
// The Receiver ignores the second stop bit. An FE (Frame Error) will therefore only be detected in the cases
//  where the first stop bit is zero.
// -----------------------------config----------------------------------------------

//------------------------------comunication--------------------------------------------
// The USART Transmitter is enabled by setting the Transmit Enable (TXEN) bit in the UCSRnB Register
//  The CPU can load the transmit buffer by writing to the UDRn I/O location
//------------------------------comunication--------------------------------------------

volatile Midi_t Midi_Variable;

// essa função recebe um canal midi e entrega esses dados a uma estrutura igual
// que está declarada globalmente para ser usada pelo periférico e sua interrupção,
// habilitando a transmição dos dados contidos no midi para o tx do atmega
uint8 play(Midi_t *midi)
{
    if(issetBit(UCSR0A, UDRE0)) {
        Midi_Variable.DATA_BYTE1 = (midi->DATA_BYTE1);
        Midi_Variable.DATA_BYTE2 = (midi->DATA_BYTE2);
        Midi_Variable.DATA_SENT = (midi->DATA_SENT);
        Midi_Variable.MIDI_CHANEL = (midi->MIDI_CHANEL);
        Midi_Variable.STATUS_BYTE = (midi->STATUS_BYTE);
        setBit(UCSR0B, TXEN0);  //habilita a transmição;
        setBit(UCSR0B, UDRIE0); // enables interrupt on the UDREn
        return 1;
    }
    return 0;
}


// choose the chanel between 0 up to 15
// choose the pin of the microcontroler
// chose the timmer beteewn 0,1 and 2
void init_midi(Midi_t *midi, uint8 chanel)
{
    midi->MIDI_CHANEL = chanel;
//-----------------------Baud_Rate--------------------------------------------
    usart0.init();
    usart0.enableTransmitter();
    UBRR0 = 31;
    setBit(UCSR0B, TXEN0);  //habilita a transmição;

//-----------------------Baud_Rate--------------------------------------------

    setBit(DDRD, PD5);  //HIGH will activate MIDI
    setBit(PORTD, PD5); //HIGH will activate MIDI

    setBit(DDRD, PD4);  //Tied to VS1053 Reset line resetMIDI
    setBit(PORTD, PD4); //Tied to VS1053 Reset line resetMIDI

    clrBit(PORTD, PD4); //habilita o modo mid do shield MP3
    delayMs(100);
    setBit(PORTD, PD4); //habilita o modo mid do shield MP3
    delayMs(100);
}

// mensagem para tocar uma nota em específico, ela recebe o canal midi,
// a nota (pitch), a oitava em que essa nota está (mais aguda ou grave que
// a escala central, pode ser positivo ou negativo), e a velocidade que
//foi tocada a nota (intensidade sonora) são enviados então 3 bytes,
// um de status (tocar nota) e os dois próximos são para informar a nota
// e a velocidade
bool note_on(Midi_t *midi, uint8 relative_pitch, int8 oitava_ave, uint8 velocidade_city)
{
    midi->STATUS_BYTE = (((0b10010000) | midi->MIDI_CHANEL)); // note on
    uint8 pitch = (relative_pitch) + 12 * oitava_ave + 60;
    if((velocidade_city < 0b10000000) & (pitch < 0b10000000)) {
        midi->DATA_BYTE1 = (pitch);
        midi->DATA_BYTE2 = (velocidade_city);
        setBit(midi->DATA_SENT, 0);         // setando a quantidade de bytes
        setBit(midi->DATA_SENT, 1);         //
        setBit(midi->DATA_SENT, 2);         // note_on
        play(midi);
        delayMs(1);
        return 1;
    } else {
        return 0;
    }
}

// pelo padrão midi, uma mensagem note_off pode ser representada como
// a nota a ser tocada, porém com o volume zerado (velocidade = 0)
void note_off(Midi_t *midi, uint8 relative_pitch, int8 oitava_ave)
{
    note_on(midi, relative_pitch, oitava_ave, 0);
}

// troca o instrumento por mandar uma mensagem via tx do atmega
// usando 2 bytes com nenhum bit de paridade e um stopbit, primeiro byte de
// status (Status_byte) e depois o instrumento (Data_byte)
void change_instrument(Midi_t *midi, uint8 instrument)
{
    midi->STATUS_BYTE = (0b11000000 | (midi->MIDI_CHANEL & 0b00001111));
    // Chanel
    midi->DATA_BYTE1 = (0b01111111 & instrument); // Instrument
    clrBit(midi->DATA_SENT, 0);         // setando a quantidade de bytes
    setBit(midi->DATA_SENT, 1);         //
    setBit(midi->DATA_SENT, 3);         // change_instrument
    play(midi);
    delayMs(1);
}

//void play_major_chord();

vbool_t printRawValue = false;

//volatile uint8 bufer_notas[24];
// MIDI configuration desligar as notas
//volatile uint8 buffer_count = 0;

int main()
{

    // Local variables
    Keypad keypad;
    uint8_t keyPressed;


    uint8_t AccelX;
    uint8_t AccelY;
    uint8_t AccelZ;
    int8_t  AccelXConv;
    int8_t  AccelYConv;
    int8_t  AccelZConv;

    uint8_t instrumento = 0;


    // MIDI configuration
    Midi_t midi;
    init_midi(&midi, 0);
    sei();

    uint8 oitava_ = 0;
    uint8 velocidade_ = fff;
    //uint32_t sustentacao = 78125; //tempo de sustentação da nota
    // (0,5 s converção)
    //  t = 1024*(sustentacao)[vai até 2^16, ao todo 67M]/(16M)
    // máximo de 4,2 segundos.

    //timer2.init(Timer2::Mode::CTC_OCRA, Timer2::ClockSource::PRESCALER_1024);
    //timer2.setCompareAValue(sustentacao);

    // MIDI configuration

    // Configuração do teclado matrixial
    keypad.setPorts(&DDRC, PC0, &DDRB, PB0);
    keypad.setKeyValues(
            Keypad::Type::KEYPAD_4X4,
            0x00, 0x01, 0x02, 0x03,
            0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0A, 0x0B,
            0x0C, 0x0D, 0x0E, 0x0F
    );
    keypad.init(5);

    twi.init(10000);

    twi.setDevice(MPU9250_SLAVEADRESS); // endereço do módulo
//    // envia o que esta no buffer para o registrador do módulo
    // nao usa twi.writeReg();
//    // lê o que esta no buffer para o registrador do módulo

    // Lê os valores dos registradores high do módulo
    twi.readReg(0x3B, &AccelX, 1);
    twi.readReg(0x3D, &AccelY, 1);
    twi.readReg(0x3F, &AccelZ, 1);

    while(1) {

        twi.readReg(0x3B, &AccelX, 1);
        twi.readReg(0x3D, &AccelY, 1);
        twi.readReg(0x3F, &AccelZ, 1);
        // Converte os valores do acelerômetro para valores com polaridade
        AccelXConv = (int8_t)AccelX;
        AccelYConv = (int8_t)AccelY;
        AccelZConv = (int8_t)AccelZ;

        // valores para instrumento 0; 16 orgao; 46 harpa; 79 ocarina;
        //                          26 guitarra jazz; 19 orgam igreja;
        // Seleciona o instrumento de acordo com o acelerômetro
        if(AccelZConv > 49) {
            instrumento = 0;
        }
        if(AccelZConv < -49) {
            instrumento = 16;
        }
        if(AccelYConv > 49) {
            instrumento = 19;
        }
        if(AccelYConv < -49) {
            instrumento = 26;
        }
        if(AccelXConv > 49) {
            instrumento = 46;
        }
        if(AccelXConv < -49) {
            instrumento = 79;
        }

        //change_instrument(&midi, instrumento);

        keypad.readKeyPressed(&keyPressed);
        switch(keyPressed) {
        case 0x00:// de 0x00 a 0x0B toca as notas
            note_on(&midi, C, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, C, oitava_);
            break;
        case 0x01:
            note_on(&midi, C_s, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, C_s, oitava_);
            break;
        case 0x02:
            note_on(&midi, D, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, D, oitava_);
            break;
        case 0x03:
            note_on(&midi, D_s, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, D_s, oitava_);
            break;
        case 0x04:
            note_on(&midi, E, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, E, oitava_);
            break;
        case 0x05:
            note_on(&midi, F, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, F, oitava_);
            break;
        case 0x06:
            note_on(&midi, F_s, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, F_s, oitava_);
            break;
        case 0x07:
            note_on(&midi, G, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, G, oitava_);
            break;
        case 0x08:
            note_on(&midi, G_s, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, G_s, oitava_);
            break;
        case 0x09:
            note_on(&midi, A, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, A, oitava_);
            break;
        case 0x0A:
            note_on(&midi, A_s, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, A_s, oitava_);
            break;
        case 0x0B:
            note_on(&midi, B, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, B, oitava_);
            break;
        case 0x0C : // Muda o instrumento
            change_instrument(&midi, instrumento);
            break;
        case 0x0D:
            // Jingle Bells, Jingle Bells
            note_on(&midi, E, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, E, oitava_);

            note_on(&midi, E, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, E, oitava_);

            note_on(&midi, E, oitava_, velocidade_);
            delayMs(1000);
            note_off(&midi, E, oitava_);

            // Jingle all the way
            note_on(&midi, E, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, E, oitava_);

            note_on(&midi, E, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, E, oitava_);

            note_on(&midi, E, oitava_, velocidade_);
            delayMs(1000);
            note_off(&midi, E, oitava_);

            // Oh, what fun it is to ride
            note_on(&midi, G, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, G, oitava_);

            note_on(&midi, A, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, A, oitava_);

            note_on(&midi, A, oitava_, velocidade_);
            delayMs(1000);
            note_off(&midi, A, oitava_);

            // In a one-horse open sleigh
            note_on(&midi, A, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, A, oitava_);

            note_on(&midi, G, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, G, oitava_);

            note_on(&midi, F, oitava_, velocidade_);
            delayMs(1000);
            note_off(&midi, F, oitava_);
            break;
        case 0x0E:
            // Brilha, brilha, estrelinha
            note_on(&midi, C, oitava_, velocidade_);
            delayMs(600);
            note_off(&midi, C, oitava_);

            note_on(&midi, C, oitava_, velocidade_);
            delayMs(600);
            note_off(&midi, C, oitava_);

            note_on(&midi, G, oitava_, velocidade_);
            delayMs(600);
            note_off(&midi, G, oitava_);

            note_on(&midi, G, oitava_, velocidade_);
            delayMs(600);
            note_off(&midi, G, oitava_);

            note_on(&midi, A, oitava_, velocidade_);
            delayMs(600);
            note_off(&midi, A, oitava_);

            note_on(&midi, A, oitava_, velocidade_);
            delayMs(600);
            note_off(&midi, A, oitava_);

            // Brilha, brilha, estrelinha
            note_on(&midi, G, oitava_, velocidade_);
            delayMs(600);
            note_off(&midi, G, oitava_);

            // Lá no alto é que está
            note_on(&midi, F, oitava_, velocidade_);
            delayMs(600);
            note_off(&midi, F, oitava_);

            // Como um diamante no céu
            note_on(&midi, F, oitava_, velocidade_);
            delayMs(600);
            note_off(&midi, F, oitava_);

            note_on(&midi, F, oitava_, velocidade_);
            delayMs(600);
            note_off(&midi, F, oitava_);

            note_on(&midi, E, oitava_, velocidade_);
            delayMs(600);
            note_off(&midi, E, oitava_);

            // Brilha, brilha, estrelinha
            note_on(&midi, E, oitava_, velocidade_);
            delayMs(600);
            note_off(&midi, E, oitava_);

            // Lá no alto é que está
            note_on(&midi, D, oitava_, velocidade_);
            delayMs(600);
            note_off(&midi, D, oitava_);

            // C note no final para dar uma pausa
            note_on(&midi, C, oitava_, velocidade_);
            delayMs(1200);
            note_off(&midi, C, oitava_);
            break;
        case 0x0F:
            // A Barata Diz Que Tem
            note_on(&midi, A, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, A, oitava_);

            note_on(&midi, G, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, G, oitava_);

            note_on(&midi, F, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, F, oitava_);

            note_on(&midi, E, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, E, oitava_);

            note_on(&midi, D, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, D, oitava_);

            note_on(&midi, C, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, C, oitava_);

            // A barata diz que tem
            note_on(&midi, A, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, A, oitava_);

            note_on(&midi, G, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, G, oitava_);

            note_on(&midi, F, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, F, oitava_);

            note_on(&midi, E, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, E, oitava_);

            note_on(&midi, D, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, D, oitava_);

            note_on(&midi, C, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, C, oitava_);

            // A barata diz que tem
            note_on(&midi, A, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, A, oitava_);

            note_on(&midi, G, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, G, oitava_);

            note_on(&midi, F, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, F, oitava_);

            note_on(&midi, E, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, E, oitava_);

            note_on(&midi, D, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, D, oitava_);

            note_on(&midi, C, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, C, oitava_);

            // E o homem diz que não tem
            note_on(&midi, G, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, G, oitava_);

            note_on(&midi, A, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, A, oitava_);

            note_on(&midi, G, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, G, oitava_);

            note_on(&midi, F, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, F, oitava_);

            note_on(&midi, E, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, E, oitava_);

            note_on(&midi, D, oitava_, velocidade_);
            delayMs(500);
            note_off(&midi, D, oitava_);

            note_on(&midi, C, oitava_, velocidade_);
            delayMs(1000);
            note_off(&midi, C, oitava_);
            break;
        default:
            break;
        }
    }
    return 0;
}


// os returns em cada instrução foram usados para que a função não tentasse
// transmitir outro dado para o UDR0 sem esse estar zerado denovo
void usartTransmissionBufferEmptyCallback()
{
    // De acordo com o valor em Midi_Variable.DATA_SENT manda os dados em Midi_Variable.STATUS_BYTE
    // para noteon e noteoff manda 3 dados, para troca de instrumento manda 2 dados
    if((Midi_Variable.DATA_SENT & 0b111) >= 4) {
        UDR0 = Midi_Variable.STATUS_BYTE;
        Midi_Variable.DATA_SENT = Midi_Variable.DATA_SENT - 4;
        return ;
    }
    if((Midi_Variable.DATA_SENT & 0b111) == 3) {
        UDR0 = Midi_Variable.DATA_BYTE1;
        Midi_Variable.DATA_SENT = Midi_Variable.DATA_SENT - 2;
        return;
    }
    if((Midi_Variable.DATA_SENT & 0b111) == 2) {
        UDR0 = Midi_Variable.DATA_BYTE1;
        Midi_Variable.DATA_SENT = 0;
        clrBit(UCSR0B, UDRIE0); // disenables interrupt on the UDREn
        return;
    }
    if((Midi_Variable.DATA_SENT & 0b111) == 1) {
        UDR0 = Midi_Variable.DATA_BYTE2;
        Midi_Variable.DATA_SENT = 0;
        clrBit(UCSR0B, UDRIE0); // disenables interrupt on the UDREn
        return;
    }
}
