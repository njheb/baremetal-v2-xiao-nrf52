// microbian.h
// Copyright (c) 2018 J. M. Spivey

typedef unsigned char byte;

#define NULL ((void *) 0)

/* Standard pids */
#define HARDWARE -1
#define IDLE 0
#define ANY -1

/* Common message types */
#define INTERRUPT 1
#define REPLY 2
#define REGISTER 3
#define PING 4
#define REQUEST 5
#define READ 7
#define WRITE 8
#define OK 9
#define ERROR 10
#define SEND 11
#define RECEIVE 12

/* Possible priorities */
#define P_HANDLER 0             // Interrupt handler
#define P_HIGH 1                // Responsive
#define P_LOW 2                 // Normal
#define P_IDLE 3                // The idle process
#define NPRIO 3                 // Number of non-idle priorities

typedef struct {                // 16 bytes
    unsigned short m_type;      // Type of message
    unsigned short m_sender;    // PID of sender
    union {                     // Three words of data, each
        int m_i;                // ... an integer
        void *m_p;              // ... or some kind of pointer
        struct {                // ... or four bytes
            byte m_bw, m_bx, m_by, m_bz;
        } m_b;
    } m_x1, m_x2, m_x3;
} message;

#define m_i1 m_x1.m_i
#define m_i2 m_x2.m_i
#define m_i3 m_x3.m_i
#define m_p1 m_x1.m_p
#define m_p2 m_x2.m_p
#define m_p3 m_x3.m_p
#define m_b1 m_x1.m_b.m_bw
#define m_b2 m_x1.m_b.m_bx
#define m_b3 m_x1.m_b.m_by
#define m_b4 m_x1.m_b.m_bz


/* microbian.c */

/* start -- create process that will run when init returns; return PID */
int start(char *name, void (*body)(int), int arg, int stksize);

#define STACK 1024              // Default stack size

/* System calls */
void yield(void);
void send(int dst, int type, message *msg);
void receive(int type, message *msg);
void sendrec(int dst, int type, message *msg);
void connect(int irq);
void priority(int p);
void exit(void);
void dump(void);

/* interrupt -- send INTERRUPT message from handler */
void interrupt(int pid);

/* lock -- disable all interrupts */
void lock(void);

/* unlock -- enable interrupts again */
void unlock(void);

/* restore -- restore previous interrupt setting (used by kprintf) */
void restore(void);


/* kprintf -- print message on console without using serial task */
void kprintf(char *fmt, ...);

/* panic -- crash with message [and show seven stars] */
void panic(char *fmt, ...);

/* badmesg -- panic after receiving unexpected message */
void badmesg(int type);

#define assert(p) \
    do { if (!(p)) panic("File %s, line %d: assertion %s failed", \
                         __FILE__, __LINE__, #p); } while (0)

/* spin -- flash the seven stars of death forever */
void spin(void);


/* serial.c */
void serial_putc(char ch);
char serial_getc(void);
void serial_init(void);

/* timer.c */
void timer_delay(int msec);
void timer_pulse(int msec);
void timer_wait(void);
unsigned timer_now(void);
unsigned timer_micros(void);
void timer_init(void);

/* i2c.c */
int i2c_probe(int chan, int addr);
int i2c_read_reg(int chan, int addr, int cmd);
void i2c_write_reg(int chan, int addr, int cmd, int val);
void i2c_read_bytes(int chan, int addr, int cmd, byte *buf, int n);
void i2c_write_bytes(int chan, int addr, int cmd, byte *buf, int n);
int i2c_xfer(int chan, int kind, int addr,
             byte *buf1, int n1, byte *buf2, int n2);
void i2c_init(int chan);

/* radio.c */
#define RADIO_PACKET 32

void radio_group(int group);
void radio_send(void *buf, int n);
int radio_receive(void *buf);
void radio_init(void);

/* display.c */
void display_show(const unsigned *img);
void display_init(void);

void image_clear(unsigned *img);
void image_set(int x, int y, unsigned *img);
