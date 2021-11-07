

enum freq {freq1, freq32, freq64, freq128};

void timer_init(freq f);
void timer_handled(void);
bool is_timer_triggered(void);
ISR(TCA0_CMP0_vect);