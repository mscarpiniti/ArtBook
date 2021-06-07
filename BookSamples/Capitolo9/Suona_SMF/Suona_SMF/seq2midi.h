// wait for a time
void wait_until(double time);

// count for elapsed time
double time_elapsed();

// play a sequence to a midi stream
void seq2midi(Alg_seq &seq, PortMidiStream *midi);

// open a stream, play a sequence, close the stream
void seq_play(Alg_seq &seq);
