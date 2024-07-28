

int	FAR PASCAL OpenChannel(LPDV, LPSTR, LPDOCINFO);
void	FAR PASCAL CloseChannel(LPDV);
void	FAR PASCAL FlushChannel(LPDV);
void	FAR PASCAL WriteChannel(LPDV, LPSTR, unsigned int);
void	FAR PASCAL WriteChannelChar(LPDV, BYTE);
void	FAR PrintChannel(LPDV, LPSTR, ...);     /* Variable parameter count! */
LPSTR FAR PASCAL farGetDecimal(LPSTR lsz, int FAR *lpiDigits);
