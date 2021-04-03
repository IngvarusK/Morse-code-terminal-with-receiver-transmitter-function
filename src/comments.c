//		if(letter_flag){
//			
//			if(letter_LCD < 16){
//				LCD1602_SetCursor(letter_LCD,0);
//				LCD1602_Write8(letter,1);
//				data[letter_LCD] = letter;
//				letter_LCD++;
//			}
//			else{
//				if(letter_LCD2 == 16) letter_LCD2 = 0;
//				LCD1602_SetCursor(0,0);
//				data[letter_LCD2] = letter;
//				for(int i = letter_LCD2+1; i < 16; i++){
//					LCD1602_Write8(data[i],1);
//				}
//				for(int i = 0; i <= letter_LCD2; i++){
//					LCD1602_Write8(data[i],1);
//				}
//				letter_LCD2++;
//			}
//			while(!(UART0->S1 & UART0_S1_TDRE_MASK));
//			UART0->D = letter;
//			letter_flag = 0;
//		}
//		
//		if(rx_EVENT_flag){
//			
//			if(letter_LCD_RX < 16){
//				LCD1602_SetCursor(letter_LCD_RX,1);
//				LCD1602_Write8(temp,1);
//				data2[letter_LCD_RX] = temp;
//				letter_LCD_RX++;
//			}
//			else{
//				if(letter_LCD2_RX == 16) letter_LCD2_RX = 0;
//				LCD1602_SetCursor(0,1);
//				data2[letter_LCD2_RX] = temp;
//				for(int i = letter_LCD2_RX+1; i < 16; i++){
//					LCD1602_Write8(data2[i],1);
//				}
//				for(int i = 0; i <= letter_LCD2_RX; i++){
//					LCD1602_Write8(data2[i],1);
//				}
//				letter_LCD2_RX++;
//			}
//			rx_EVENT_flag = 0;
//		}
