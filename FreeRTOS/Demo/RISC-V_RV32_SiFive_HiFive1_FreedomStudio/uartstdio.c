/*
 * uartstdio.c
 *
 *  Created on: Jan 14, 2020
 *      Author: HoangSHC
 */

#include <stdint.h>
#include <stdarg.h>	/* va_list, va_arg() */
#include "uart.h"

//*****************************************************************************
//
// A mapping from an integer between 0 and 15 to its ASCII character
// equivalent.
//
//*****************************************************************************
static const char * const g_pcHex = "0123456789abcdef";


//*****************************************************************************
//
//! A simple UART based get string function, with some line processing.
//!
//! \param pcBuf points to a buffer for the incoming string from the UART.
//! \param ui32Len is the length of the buffer for storage of the string,
//! including the trailing 0.
//!
//! This function will receive a string from the UART input and store the
//! characters in the buffer pointed to by \e pcBuf.  The characters will
//! continue to be stored until a termination character is received.  The
//! termination characters are CR, LF, or ESC.  A CRLF pair is treated as a
//! single termination character.  The termination characters are not stored in
//! the string.  The string will be terminated with a 0 and the function will
//! return. This function will block until a termination character is received.
//!
//! Since the string will be null terminated, the user must ensure that the
//! buffer is sized to allow for the additional null character.
//!
//! \return Returns the count of characters that were stored, not including
//! the trailing 0.
//
//*****************************************************************************
int uart_gets(char *pcBuf, uint32_t ui32Len)
{
    uint32_t ui32Count = 0;
    int8_t cChar;
    static int8_t bLastWasCR = 0;

    // Check the arguments.
    if(!pcBuf)
    	return -1;

    // Adjust the length back by 1 to leave space for the trailing
    // null terminator.
    ui32Len--;

    // Process characters until a newline is received.
    while(1)
    {
        cChar = uart_getc(); // Read the next character from the console.

        // See if the backspace key was pressed.
        if(cChar == '\b')
        {
            // If there are any characters already in the buffer, then delete
            // the last.
            if(ui32Count)
            {
                uart_puts("\b \b");  // Rub out the previous character.
                ui32Count--;  // Decrement the number of characters in the buffer.
            }
            continue;  // Skip ahead to read the next character.
        }

        // If this character is LF and last was CR, then just gobble up the
        // character because the EOL processing was taken care of with the CR.
        if((cChar == '\n') && bLastWasCR)
        {
            bLastWasCR = 0;
            continue;
        }

        // See if a newline or escape character was received.
        if((cChar == '\r') || (cChar == '\n') || (cChar == 0x1b))
        {
            // If the character is a CR, then it may be followed by a LF which
            // should be paired with the CR.  So remember that a CR was
            // received.
            if(cChar == '\r')
            {
                bLastWasCR = 1;
            }
            break;  // Stop processing the input and end the line.
        }

        // Process the received character as long as we are not at the end of
        // the buffer.  If the end of the buffer has been reached then all
        // additional characters are ignored until a newline is received.
        if(ui32Count < ui32Len)
        {
            pcBuf[ui32Count] = cChar;  // Store the character in the caller supplied buffer.
            ui32Count++;  // Increment the count of characters received.
            uart_putc(cChar);  // Reflect the character back to the user.
        }
    }

    pcBuf[ui32Count] = 0;  // Add a null termination to the string.
    uart_puts("\r\n");  // Send a CRLF pair to the terminal to end the line.

    return(ui32Count);  // Return the count of int8_ts in the buffer, not counting the trailing 0.
}

int uart_write(const char *pcBuf, uint32_t ui32Len)
{
    unsigned int uIdx;

    // Check for valid arguments.
    if(!pcBuf)
    	return 0;

    // Send the characters
    for(uIdx = 0; uIdx < ui32Len; uIdx++)
    {
        // If the character to the UART is \n, then add a \r before it so that
        // \n is translated to \n\r in the output.
        if(pcBuf[uIdx] == '\n')
        {
            uart_putc('\r');
        }

        uart_putc(pcBuf[uIdx]);  // Send the character to the UART output.
    }

    return(uIdx);  // Return the number of characters written.
}

//*****************************************************************************
//
//! A simple UART based vprintf function supporting \%c, \%d, \%p, \%s, \%u,
//! \%x, and \%X.
//!
//! \param pcString is the format string.
//! \param vaArgP is a variable argument list pointer whose content will depend
//! upon the format string passed in \e pcString.
//!
//! This function is very similar to the C library <tt>vprintf()</tt> function.
//! All of its output will be sent to the UART.  Only the following formatting
//! characters are supported:
//!
//! - \%c to print a character
//! - \%d or \%i to print a decimal value
//! - \%s to print a string
//! - \%u to print an unsigned decimal value
//! - \%x to print a hexadecimal value using lower case letters
//! - \%X to print a hexadecimal value using lower case letters (not upper case
//! letters as would typically be used)
//! - \%p to print a pointer as a hexadecimal value
//! - \%\% to print out a \% character
//!
//! For \%s, \%d, \%i, \%u, \%p, \%x, and \%X, an optional number may reside
//! between the \% and the format character, which specifies the minimum number
//! of characters to use for that value; if preceded by a 0 then the extra
//! characters will be filled with zeros instead of spaces.  For example,
//! ``\%8d'' will use eight characters to print the decimal value with spaces
//! added to reach eight; ``\%08d'' will use eight characters as well but will
//! add zeroes instead of spaces.
//!
//! The type of the arguments in the variable arguments list must match the
//! requirements of the format string.  For example, if an integer was passed
//! where a string was expected, an error of some kind will most likely occur.
//!
//! \return None.
//
//*****************************************************************************
void uart_vprintf(const char *pcString, va_list vaArgP)
{
    uint32_t ui32Idx, ui32Value, ui32Pos, ui32Count, ui32Base, ui32Neg;
    char *pcStr, pcBuf[16], cFill;

    // Check the arguments.
    if(pcString == 0)
    	return;

    // Loop while there are more characters in the string.
    while(*pcString)
    {
        // Find the first non-% character, or the end of the string.
        for(ui32Idx = 0;
            (pcString[ui32Idx] != '%') && (pcString[ui32Idx] != '\0');
            ui32Idx++)
        {

        }

        uart_write(pcString, ui32Idx);  // Write this portion of the string.
        pcString += ui32Idx;  // Skip the portion of the string that was written.

        // See if the next character is a %.
        if(*pcString == '%')
        {
            pcString++;  // Skip the %.

            // Set the digit count to zero, and the fill character to space
            // (in other words, to the defaults).
            ui32Count = 0;
            cFill = ' ';

            // It may be necessary to get back here to process more characters.
            // Goto's aren't pretty, but effective.  I feel extremely dirty for
            // using not one but two of the beasts.
again:
            // Determine how to handle the next character.
            switch(*pcString++)
            {
                // Handle the digit characters.
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                {
                    // If this is a zero, and it is the first digit, then the
                    // fill character is a zero instead of a space.
                    if((pcString[-1] == '0') && (ui32Count == 0))
                    {
                        cFill = '0';
                    }

                    // Update the digit count.
                    ui32Count *= 10;
                    ui32Count += pcString[-1] - '0';
                    goto again;  // Get the next character.
                }

                // Handle the %c command.
                case 'c':
                {
                    ui32Value = va_arg(vaArgP, uint32_t);  // Get the value from the varargs.
                    uart_write((char *)&ui32Value, 1);  // Print out the character.
                    break;  // This command has been handled.
                }

                // Handle the %d and %i commands.
                case 'd':
                case 'i':
                {
                    ui32Value = va_arg(vaArgP, uint32_t);  // Get the value from the varargs.
                    ui32Pos = 0;  // Reset the buffer position.

                    // If the value is negative, make it positive and indicate
                    // that a minus sign is needed.
                    if((int32_t)ui32Value < 0)
                    {
                        ui32Value = -(int32_t)ui32Value;  // Make the value positive.
                        ui32Neg = 1;  // Indicate that the value is negative.
                    }
                    else
                    {
                        ui32Neg = 0;  // Indicate that the value is positive so that a minus sign isn't inserted.
                    }

                    ui32Base = 10;  // Set the base to 10.
                    goto convert;  // Convert the value to ASCII.
                }

                // Handle the %s command.
                case 's':
                {
                    pcStr = va_arg(vaArgP, char *);  // Get the string pointer from the varargs.

                    // Determine the length of the string.
                    for(ui32Idx = 0; pcStr[ui32Idx] != '\0'; ui32Idx++);

                    uart_write(pcStr, ui32Idx);  // Write the string.

                    // Write any required padding spaces
                    if(ui32Count > ui32Idx)
                    {
                        ui32Count -= ui32Idx;
                        while(ui32Count--)
                        {
                            uart_write(" ", 1);
                        }
                    }
                    break;  // This command has been handled.
                }

                // Handle the %u command.
                case 'u':
                {
                    ui32Value = va_arg(vaArgP, uint32_t);  // Get the value from the varargs.
                    ui32Pos = 0;  // Reset the buffer position.
                    ui32Base = 10;  // Set the base to 10.
                    ui32Neg = 0;  // Indicate that the value is positive so that a minus sign isn't inserted.
                    goto convert;  // Convert the value to ASCII.
                }

                // Handle the %x and %X commands.  Note that they are treated
                // identically; in other words, %X will use lower case letters
                // for a-f instead of the upper case letters it should use.  We
                // also alias %p to %x.
                case 'x':
                case 'X':
                case 'p':
                {
                    ui32Value = va_arg(vaArgP, uint32_t);  // Get the value from the varargs.
                    ui32Pos = 0;  // Reset the buffer position.
                    ui32Base = 16;  // Set the base to 16.
                    ui32Neg = 0;  // Indicate that the value is positive so that a minus sign isn't inserted.

                    // Determine the number of digits in the string version of
                    // the value.
convert:
                    for(ui32Idx = 1;
                        (((ui32Idx * ui32Base) <= ui32Value) &&
                         (((ui32Idx * ui32Base) / ui32Base) == ui32Idx));
                        ui32Idx *= ui32Base, ui32Count--)
                    {
                    }

                    // If the value is negative, reduce the count of padding
                    // characters needed.
                    if(ui32Neg)
                    {
                        ui32Count--;
                    }

                    // If the value is negative and the value is padded with
                    // zeros, then place the minus sign before the padding.
                    if(ui32Neg && (cFill == '0'))
                    {
                        pcBuf[ui32Pos++] = '-';  // Place the minus sign in the output buffer.
                        ui32Neg = 0;  // The minus sign has been placed, so turn off the negative flag.
                    }

                    // Provide additional padding at the beginning of the
                    // string conversion if needed.
                    if((ui32Count > 1) && (ui32Count < 16))
                    {
                        for(ui32Count--; ui32Count; ui32Count--)
                        {
                            pcBuf[ui32Pos++] = cFill;
                        }
                    }

                    // If the value is negative, then place the minus sign
                    // before the number.
                    if(ui32Neg)
                    {
                        pcBuf[ui32Pos++] = '-';  // Place the minus sign in the output buffer.
                    }

                    // Convert the value into a string.
                    for(; ui32Idx; ui32Idx /= ui32Base)
                    {
                        pcBuf[ui32Pos++] =
                            g_pcHex[(ui32Value / ui32Idx) % ui32Base];
                    }

                    uart_write(pcBuf, ui32Pos);  // Write the string.
                    break;  // This command has been handled.
                }

                // Handle the %% command.
                case '%':
                {
                    uart_write(pcString - 1, 1);  // Simply write a single %.
                    break;  // This command has been handled.
                }

                // Handle all other commands.
                default:
                {
                    uart_write("ERROR", 5);  // Indicate an error.
                    break;  // This command has been handled.
                }
            }
        }
    }
}

//*****************************************************************************
//
//! A simple UART based printf function supporting \%c, \%d, \%p, \%s, \%u,
//! \%x, and \%X.
//!
//! \param pcString is the format string.
//! \param ... are the optional arguments, which depend on the contents of the
//! format string.
//!
//! This function is very similar to the C library <tt>fprintf()</tt> function.
//! All of its output will be sent to the UART.  Only the following formatting
//! characters are supported:
//!
//! - \%c to print a character
//! - \%d or \%i to print a decimal value
//! - \%s to print a string
//! - \%u to print an unsigned decimal value
//! - \%x to print a hexadecimal value using lower case letters
//! - \%X to print a hexadecimal value using lower case letters (not upper case
//! letters as would typically be used)
//! - \%p to print a pointer as a hexadecimal value
//! - \%\% to print out a \% character
//!
//! For \%s, \%d, \%i, \%u, \%p, \%x, and \%X, an optional number may reside
//! between the \% and the format character, which specifies the minimum number
//! of characters to use for that value; if preceded by a 0 then the extra
//! characters will be filled with zeros instead of spaces.  For example,
//! ``\%8d'' will use eight characters to print the decimal value with spaces
//! added to reach eight; ``\%08d'' will use eight characters as well but will
//! add zeroes instead of spaces.
//!
//! The type of the arguments after \e pcString must match the requirements of
//! the format string.  For example, if an integer was passed where a string
//! was expected, an error of some kind will most likely occur.
//!
//! \return None.
//
//*****************************************************************************
void uart_printf(const char *pcString, ...)
{
    va_list vaArgP;

    va_start(vaArgP, pcString);  // Start the varargs processing.
    uart_vprintf(pcString, vaArgP);
    va_end(vaArgP);  // We're finished with the varargs now.
}
