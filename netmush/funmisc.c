/**
 * @file funmisc.c
 * @author TinyMUSH development team (https://github.com/TinyMUSH)
 * @brief Misc functions
 * @version 3.3
 * @date 2021-01-04
 * 
 * @copyright Copyright (C) 1989-2021 TinyMUSH development team.
 *            You may distribute under the terms the Artistic License,
 *            as specified in the COPYING file.
 * 
 */

#include "system.h"

#include "defaults.h"
#include "constants.h"
#include "typedefs.h"
#include "macros.h"
#include "externs.h"
#include "prototypes.h"

MONTHDAYS mdtab[] = {
	{(char *)"Jan", 31},
	{(char *)"Feb", 29},
	{(char *)"Mar", 31},
	{(char *)"Apr", 30},
	{(char *)"May", 31},
	{(char *)"Jun", 30},
	{(char *)"Jul", 31},
	{(char *)"Aug", 31},
	{(char *)"Sep", 30},
	{(char *)"Oct", 31},
	{(char *)"Nov", 30},
	{(char *)"Dec", 31}};

/**
 * @brief   The switchall() function compares <str> against <pat1>, <pat2>, etc.
 *          (allowing * to match any number of characters and ? to match any 1
 *          character), and returns the corresponding <resN> parameters (without
 *          any delimiters) for all <patN> patterns that match.  If none match,
 *          then the default result <dflt> is returned. The evaluated value of
 *          <str> can be obtained as '#$'. If switchall() and switch() are
 *          nested, the nest level can be obtained with '#!'.
 * 
 * @note This functions expect that its arguments has not been evaluated.
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player DBref of player
 * @param caller DBref of caller
 * @param cause DBref of cause
 * @param fargs Function's arguments
 * @param nfargs Number of function's arguments
 * @param cargs Command's arguments
 * @param ncargs Nomber of command's arguments
 */
void fun_switchall(char *buff, char **bufc, dbref player, dbref caller, dbref cause, char *fargs[], int nfargs, char *cargs[], int ncargs)
{
	int i = 0;
	bool got_one = false;
	char *mbuff = NULL, *tbuff = NULL, *bp = NULL, *str = NULL, *save_token = NULL;

	/**
     * If we don't have at least 2 args, return nothing
	 * 
     */
	if (nfargs < 2)
	{
		return;
	}

	/**
     * Evaluate the target in fargs[0]
	 * 
     */
	mbuff = bp = XMALLOC(LBUF_SIZE, "mbuff");
	str = fargs[0];
	exec(mbuff, &bp, player, caller, cause, EV_STRIP | EV_FCHECK | EV_EVAL, &str, cargs, ncargs);

	/**
     * Loop through the patterns looking for a match
	 * 
     */
	mushstate.in_switch++;
	save_token = mushstate.switch_token;

	for (i = 1; (i < nfargs - 1) && fargs[i] && fargs[i + 1]; i += 2)
	{
		tbuff = bp = XMALLOC(LBUF_SIZE, "bp");
		str = fargs[i];
		exec(tbuff, &bp, player, caller, cause, EV_STRIP | EV_FCHECK | EV_EVAL, &str, cargs, ncargs);

		if (quick_wild(tbuff, mbuff))
		{
			got_one = true;
			XFREE(tbuff);
			mushstate.switch_token = mbuff;
			str = fargs[i + 1];
			exec(buff, bufc, player, caller, cause, EV_STRIP | EV_FCHECK | EV_EVAL, &str, cargs, ncargs);
		}
		else
		{
			XFREE(tbuff);
		}
	}

	/**
     * If we didn't match, return the default if there is one
	 * 
     */
	if (!got_one && (i < nfargs) && fargs[i])
	{
		mushstate.switch_token = mbuff;
		str = fargs[i];
		exec(buff, bufc, player, caller, cause, EV_STRIP | EV_FCHECK | EV_EVAL, &str, cargs, ncargs);
	}

	XFREE(mbuff);
	mushstate.in_switch--;
	mushstate.switch_token = save_token;
}

/**
 * @brief   The switch() function compares <str> against <pat1>, <pat2>, etc (allowing
 *          to match any number of characters and ? to match any 1 character), and
 *          returns the corresponding <resN> parameter for the first <patN> pattern
 *          that matches.  If none match, then the default result <dflt> is returned.
 *          The evaluated value of <str> can be obtained as '#$'. If switch() and
 *          switchall() are nested, the nest level can be obtained with '#!'.
 * 
 * @note This functions expect that its arguments has not been evaluated.
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player DBref of player
 * @param caller DBref of caller
 * @param cause DBref of cause
 * @param fargs Function's arguments
 * @param nfargs Number of function's arguments
 * @param cargs Command's arguments
 * @param ncargs Nomber of command's arguments
 */
void fun_switch(char *buff, char **bufc, dbref player, dbref caller, dbref cause, char *fargs[], int nfargs, char *cargs[], int ncargs)
{
	int i = 0;
	char *mbuff = NULL, *tbuff = NULL, *bp = NULL, *str = NULL, *save_token = NULL;

	/**
     * If we don't have at least 2 args, return nothing
	 * 
     */
	if (nfargs < 2)
	{
		return;
	}

	/**
     * Evaluate the target in fargs[0]
	 * 
     */
	mbuff = bp = XMALLOC(LBUF_SIZE, "bp");
	str = fargs[0];
	exec(mbuff, &bp, player, caller, cause, EV_STRIP | EV_FCHECK | EV_EVAL, &str, cargs, ncargs);

	/**
     * Loop through the patterns looking for a match
	 * 
     */
	mushstate.in_switch++;
	save_token = mushstate.switch_token;

	for (i = 1; (i < nfargs - 1) && fargs[i] && fargs[i + 1]; i += 2)
	{
		tbuff = bp = XMALLOC(LBUF_SIZE, "bp");
		str = fargs[i];
		exec(tbuff, &bp, player, caller, cause, EV_STRIP | EV_FCHECK | EV_EVAL, &str, cargs, ncargs);

		if (quick_wild(tbuff, mbuff))
		{
			XFREE(tbuff);
			mushstate.switch_token = mbuff;
			str = fargs[i + 1];
			exec(buff, bufc, player, caller, cause, EV_STRIP | EV_FCHECK | EV_EVAL, &str, cargs, ncargs);
			XFREE(mbuff);
			mushstate.in_switch--;
			mushstate.switch_token = save_token;
			return;
		}

		XFREE(tbuff);
	}

	/**
     * Nope, return the default if there is one
	 * 
     */
	if ((i < nfargs) && fargs[i])
	{
		mushstate.switch_token = mbuff;
		str = fargs[i];
		exec(buff, bufc, player, caller, cause, EV_STRIP | EV_FCHECK | EV_EVAL, &str, cargs, ncargs);
	}

	XFREE(mbuff);
	mushstate.in_switch--;
	mushstate.switch_token = save_token;
}

/**
 * @brief   This function is similar to switch(), save that it looks for an exact
 *          match between the patterns and the string, rather than doing a 'wildcard'
 *          match (case-insensitive match with '*' and '?'), and the '#$' token
 *          replacement is not done. It performs marginally faster than switch().
 * 
 * @note This functions expect that its arguments has not been evaluated.
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player DBref of player
 * @param caller DBref of caller
 * @param cause DBref of cause
 * @param fargs Function's arguments
 * @param nfargs Number of function's arguments
 * @param cargs Command's arguments
 * @param ncargs Nomber of command's arguments
 */
void fun_case(char *buff, char **bufc, dbref player, dbref caller, dbref cause, char *fargs[], int nfargs, char *cargs[], int ncargs)
{
	int i = 0;
	char *mbuff = NULL, *tbuff = NULL, *bp = NULL, *str = NULL;

	/**
     * If we don't have at least 2 args, return nothing
	 * 
     */
	if (nfargs < 2)
	{
		return;
	}

	/**
     * Evaluate the target in fargs[0]
	 * 
     */
	mbuff = bp = XMALLOC(LBUF_SIZE, "bp");
	str = fargs[0];
	exec(mbuff, &bp, player, caller, cause, EV_STRIP | EV_FCHECK | EV_EVAL, &str, cargs, ncargs);

	/**
     * Loop through the patterns looking for an exact match
	 * 
     */
	for (i = 1; (i < nfargs - 1) && fargs[i] && fargs[i + 1]; i += 2)
	{
		tbuff = bp = XMALLOC(LBUF_SIZE, "bp");
		str = fargs[i];
		exec(tbuff, &bp, player, caller, cause, EV_STRIP | EV_FCHECK | EV_EVAL, &str, cargs, ncargs);

		if (!strcmp(tbuff, mbuff))
		{
			XFREE(tbuff);
			str = fargs[i + 1];
			exec(buff, bufc, player, caller, cause, EV_STRIP | EV_FCHECK | EV_EVAL, &str, cargs, ncargs);
			XFREE(mbuff);
			return;
		}

		XFREE(tbuff);
	}

	XFREE(mbuff);

	/**
     * Nope, return the default if there is one
	 * 
     */
	if ((i < nfargs) && fargs[i])
	{
		str = fargs[i];
		exec(buff, bufc, player, caller, cause, EV_STRIP | EV_FCHECK | EV_EVAL, &str, cargs, ncargs);
	}

	return;
}

/**
 * @brief Handle if else cases.
 * 
 * @note This functions expect that its arguments has not been evaluated.
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player DBref of player
 * @param caller DBref of caller
 * @param cause DBref of cause
 * @param fargs Function's arguments
 * @param nfargs Number of function's arguments
 * @param cargs Command's arguments
 * @param ncargs Nomber of command's arguments
 */
void handle_ifelse(char *buff, char **bufc, dbref player, dbref caller, dbref cause, char *fargs[], int nfargs, char *cargs[], int ncargs)
{
	char *str = NULL, *mbuff = NULL, *bp = NULL, *save_token = NULL, *tbuf = NULL;
	int n = 0, flag = Func_Flags(fargs);

	if (flag & IFELSE_DEFAULT)
	{
		if (!fn_range_check(((FUN *)fargs[-1])->name, nfargs, 1, 2, buff, bufc))
		{
			return;
		}
	}
	else
	{
		if (!fn_range_check(((FUN *)fargs[-1])->name, nfargs, 2, 3, buff, bufc))
		{
			return;
		}
	}

	mbuff = bp = XMALLOC(LBUF_SIZE, "bp");
	str = fargs[0];
	exec(mbuff, &bp, player, caller, cause, EV_STRIP | EV_FCHECK | EV_EVAL, &str, cargs, ncargs);

	/**
     * We default to bool-style, but we offer the option of the MUX-style
     * nonzero -- it's true if it's not empty or zero.
	 * 
     */
	if (!mbuff || !*mbuff)
	{
		n = 0;
	}
	else if (flag & IFELSE_BOOL)
	{
		/**
		 * xlate() destructively modifies the string
		 * 
		 */
		tbuf = XSTRDUP(mbuff, "tbuf");
		n = xlate(tbuf);
		XFREE(tbuf);
	}
	else
	{
		n = !(((int)strtol(mbuff, (char **)NULL, 10) == 0) && is_number(mbuff));
	}

	if (flag & IFELSE_FALSE)
	{
		n = !n;
	}

	if (flag & IFELSE_DEFAULT)
	{
		/*
		 * If we got our condition, return the string, otherwise
		 * return our 'else' default clause.
		 * 
		 */
		if (n)
		{
			SAFE_LB_STR(mbuff, buff, bufc);
		}
		else
		{
			str = fargs[1];
			exec(buff, bufc, player, caller, cause, EV_STRIP | EV_FCHECK | EV_EVAL, &str, cargs, ncargs);
		}

		XFREE(mbuff);
		return;
	}

	/**
     * Not default mode: Use our condition to execute result clause
	 * 
     */
	if (!n)
	{
		if (nfargs != 3)
		{
			XFREE(mbuff);
			return;
		}

		/**
		 * Do 'false' clause
		 * 
		 */
		str = fargs[2];
	}
	else
	{
		/**
		 * Do 'true' clause
		 * 
		 */
		str = fargs[1];
	}

	if (flag & IFELSE_TOKEN)
	{
		mushstate.in_switch++;
		save_token = mushstate.switch_token;
		mushstate.switch_token = mbuff;
	}

	exec(buff, bufc, player, caller, cause, EV_STRIP | EV_FCHECK | EV_EVAL, &str, cargs, ncargs);
	XFREE(mbuff);

	if (flag & IFELSE_TOKEN)
	{
		mushstate.in_switch--;
		mushstate.switch_token = save_token;
	}
}

/**
 * @brief Return a random number from 0 to arg1 - 1
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player Not used
 * @param caller Not used
 * @param cause Not used
 * @param fargs Function's arguments
 * @param nfargs Not used
 * @param cargs Not used
 * @param ncargs Not used
 */
void fun_rand(char *buff, char **bufc, dbref player __attribute__((unused)), dbref caller __attribute__((unused)), dbref cause __attribute__((unused)), char *fargs[], int nfargs __attribute__((unused)), char *cargs[] __attribute__((unused)), int ncargs __attribute__((unused)))
{
	int num = (int)strtol(fargs[0], (char **)NULL, 10);

	if (num < 1)
	{
		SAFE_LB_CHR('0', buff, bufc);
	}
	else
	{
		SAFE_SPRINTF(buff, bufc, "%ld", random_range(0, (num)-1));
	}
}

/**
 * @brief Roll XdY dice
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player Not used
 * @param caller Not used
 * @param cause Not used
 * @param fargs Function's arguments
 * @param nfargs Not used
 * @param cargs Not used
 * @param ncargs Not used
 */
void fun_die(char *buff, char **bufc, dbref player __attribute__((unused)), dbref caller __attribute__((unused)), dbref cause __attribute__((unused)), char *fargs[], int nfargs __attribute__((unused)), char *cargs[] __attribute__((unused)), int ncargs __attribute__((unused)))
{
	int n, die, count;
	int total = 0;

	if (!fargs[0] || !fargs[1])
	{
		SAFE_LB_CHR('0', buff, bufc);
		return;
	}

	n = (int)strtol(fargs[0], (char **)NULL, 10);
	die = (int)strtol(fargs[1], (char **)NULL, 10);

	if ((n == 0) || (die <= 0))
	{
		SAFE_LB_CHR('0', buff, bufc);
		return;
	}

	if ((n < 1) || (n > 100))
	{
		SAFE_LB_STR("#-1 NUMBER OUT OF RANGE", buff, bufc);
		return;
	}

	for (count = 0; count < n; count++)
	{
		total += (int)random_range(1, die);
	}

	SAFE_LTOS(buff, bufc, total, LBUF_SIZE);
}

/**
 * @brief Generate random list
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player DBref of player
 * @param caller DBref of caller
 * @param cause DBref of cause
 * @param fargs Function's arguments
 * @param nfargs Number of function's arguments
 * @param cargs Command's arguments
 * @param ncargs Nomber of command's arguments
 */
void fun_lrand(char *buff, char **bufc, dbref player, dbref caller, dbref cause, char *fargs[], int nfargs, char *cargs[], int ncargs)
{
	Delim osep;
	int n_times = 0, r_bot = 0, r_top = 0, i = 0;
	double n_range = 0.0;
	unsigned int tmp = 0;
	char *bb_p = NULL;

	/**
     * Special: the delim is really an output delim.
	 * 
     */
	if (!fn_range_check(((FUN *)fargs[-1])->name, nfargs, 3, 4, buff, bufc))
	{
		return;
	}

	if (!delim_check(buff, bufc, player, caller, cause, fargs, nfargs, cargs, ncargs, 4, &osep, DELIM_STRING | DELIM_NULL | DELIM_CRLF))
	{
		return;
	}

	/**
     * If we're generating no numbers, since this is a list function, we
     * return empty, rather than returning 0.
	 * 
     */
	n_times = (int)strtol(fargs[2], (char **)NULL, 10);

	if (n_times < 1)
	{
		return;
	}

	if (n_times > LBUF_SIZE)
	{
		n_times = LBUF_SIZE;
	}

	r_bot = (int)strtol(fargs[0], (char **)NULL, 10);
	r_top = (int)strtol(fargs[1], (char **)NULL, 10);

	if (r_top < r_bot)
	{
		/**
		 * This is an error condition. Just return an empty list. We
		 * obviously can't return a random number between X and Y if
		 * Y is less than X.
		 * 
		 */
		return;
	}
	else if (r_bot == r_top)
	{
		/**
		 * Just generate a list of n repetitions.
		 * 
		 */
		bb_p = *bufc;

		for (i = 0; i < n_times; i++)
		{
			if (*bufc != bb_p)
			{
				print_separator(&osep, buff, bufc);
			}

			SAFE_LTOS(buff, bufc, r_bot, LBUF_SIZE);
		}

		return;
	}

	/**
     * We've hit this point, we have a range. Generate a list.
	 * 
     */
	n_range = (double)r_top - r_bot + 1;
	bb_p = *bufc;

	for (i = 0; i < n_times; i++)
	{
		if (*bufc != bb_p)
		{
			print_separator(&osep, buff, bufc);
		}

		tmp = (unsigned int)random_range(0, (n_range)-1);

		SAFE_LTOS(buff, bufc, r_bot + tmp, LBUF_SIZE);
	}
}

/**
 * @brief Return a list of numbers.
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player DBref of player
 * @param caller DBref of caller
 * @param cause DBref of cause
 * @param fargs Function's arguments
 * @param nfargs Number of function's arguments
 * @param cargs Command's arguments
 * @param ncargs Nomber of command's arguments
 */
void fun_lnum(char *buff, char **bufc, dbref player, dbref caller, dbref cause, char *fargs[], int nfargs, char *cargs[], int ncargs)
{
	Delim osep;
	int bot = 0, top = 0, over = 0, i = 0;
	char *bb_p = NULL, *startp = NULL, *endp = NULL, *tbuf = NULL;
	int lnum_init = 0;
	char *lnum_buff = XMALLOC(290, "lnum_buff");

	if (nfargs == 0)
	{
		XFREE(lnum_buff);
		return;
	}

	/**
     * lnum() is special, since its single delimiter is really an output
     * delimiter.
     */
	if (!fn_range_check(((FUN *)fargs[-1])->name, nfargs, 1, 3, buff, bufc))
	{
		return;
	}

	if (!delim_check(buff, bufc, player, caller, cause, fargs, nfargs, cargs, ncargs, 3, &osep, DELIM_STRING | DELIM_NULL | DELIM_CRLF))
	{
		return;
	}

	if (nfargs >= 2)
	{
		bot = (int)strtol(fargs[0], (char **)NULL, 10);
		top = (int)strtol(fargs[1], (char **)NULL, 10);
	}
	else
	{
		bot = 0;
		top = (int)strtol(fargs[0], (char **)NULL, 10);

		if (top-- < 1)
		{
			/**
			 * still want to generate if arg is 1
			 * 
			 */
			XFREE(lnum_buff);
			return;
		}
	}

	/**
     * We keep 0-100 pre-generated so we can do quick copies.
	 * 
     */
	if (!lnum_init)
	{
		XSTRCPY(lnum_buff, (char *)"0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99");
		lnum_init = 1;
	}

	/**
     * If it's an ascending sequence crossing from negative numbers into
     * positive, get the negative numbers out of the way first.
	 * 
     */
	bb_p = *bufc;
	over = 0;

	if ((bot < 0) && (top >= 0) && (osep.len == 1) && (osep.str[0] == ' '))
	{
		while ((bot < 0) && !over)
		{
			if (*bufc != bb_p)
			{
				print_separator(&osep, buff, bufc);
			}

			tbuf = ltos(bot);
			over = SAFE_LB_STR(tbuf, buff, bufc);
			XFREE(tbuf);
			bot++;
		}

		if (over)
		{
			XFREE(lnum_buff);
			return;
		}
	}

	/**
     * Copy as much out of the pre-gen as we can.
	 * 
     */
	if ((bot >= 0) && (bot < 100) && (top > bot) && (osep.len == 1) && (osep.str[0] == ' '))
	{
		if (*bufc != bb_p)
		{
			print_separator(&osep, buff, bufc);
		}

		startp = lnum_buff + (bot < 10 ? 2 * bot : (3 * bot) - 10);

		if (top >= 99)
		{
			SAFE_LB_STR(startp, buff, bufc);
		}
		else
		{
			endp = lnum_buff + (top + 1 < 10 ? 2 * top + 1 : (3 * top + 1) - 10) - 1;
			*endp = '\0';
			SAFE_LB_STR(startp, buff, bufc);
			*endp = ' ';
		}

		if (top < 100)
		{
			XFREE(lnum_buff);
			return;
		}
		else
		{
			bot = 100;
		}
	}

	/**
     * Print a new list.
	 * 
     */
	if (top == bot)
	{
		if (*bufc != bb_p)
		{
			print_separator(&osep, buff, bufc);
		}

		SAFE_LTOS(buff, bufc, bot, LBUF_SIZE);
		XFREE(lnum_buff);
		return;
	}
	else if (top > bot)
	{
		for (i = bot; (i <= top) && !over; i++)
		{
			if (*bufc != bb_p)
			{
				print_separator(&osep, buff, bufc);
			}

			tbuf = ltos(i);
			over = SAFE_LB_STR(tbuf, buff, bufc);
			XFREE(tbuf);
		}
	}
	else
	{
		for (i = bot; (i >= top) && !over; i--)
		{
			if (*bufc != bb_p)
			{
				print_separator(&osep, buff, bufc);
			}

			tbuf = ltos(i);
			over = SAFE_LB_STR(tbuf, buff, bufc);
			XFREE(tbuf);
		}
	}
	XFREE(lnum_buff);
}

/**
 * @brief Returns nicely-formatted time.
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player Not used
 * @param caller Not used
 * @param cause Not used
 * @param fargs Not used
 * @param nfargs Not used
 * @param cargs Not used
 * @param ncargs Not used
 */
void fun_time(char *buff, char **bufc, dbref player __attribute__((unused)), dbref caller __attribute__((unused)), dbref cause __attribute__((unused)), char *fargs[] __attribute__((unused)), int nfargs __attribute__((unused)), char *cargs[] __attribute__((unused)), int ncargs __attribute__((unused)))
{
	char *temp = (char *)ctime(&mushstate.now);

	temp[strlen(temp) - 1] = '\0';
	SAFE_LB_STR(temp, buff, bufc);
}

/**
 * @brief Seconds since 0:00 1/1/70
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player Not used
 * @param caller Not used
 * @param cause Not used
 * @param fargs Not used
 * @param nfargs Not used
 * @param cargs Not used
 * @param ncargs Not used
 */
void fun_secs(char *buff, char **bufc, dbref player __attribute__((unused)), dbref caller __attribute__((unused)), dbref cause __attribute__((unused)), char *fargs[] __attribute__((unused)), int nfargs __attribute__((unused)), char *cargs[] __attribute__((unused)), int ncargs __attribute__((unused)))
{
	SAFE_LTOS(buff, bufc, mushstate.now, LBUF_SIZE);
}

/**
 * @brief Converts seconds to time string, based off 0:00 1/1/70
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player Not used
 * @param caller Not used
 * @param cause Not used
 * @param fargs Function's arguments
 * @param nfargs Not used
 * @param cargs Not used
 * @param ncargs Not used
 */
void fun_convsecs(char *buff, char **bufc, dbref player __attribute__((unused)), dbref caller __attribute__((unused)), dbref cause __attribute__((unused)), char *fargs[], int nfargs __attribute__((unused)), char *cargs[] __attribute__((unused)), int ncargs __attribute__((unused)))
{
	time_t tt = strtol(fargs[0], (char **)NULL, 10);
	char *temp = (char *)ctime(&tt);

	temp[strlen(temp) - 1] = '\0';
	SAFE_LB_STR(temp, buff, bufc);
}

/**
 * @brief Converts time string to a struct tm.
 * 
 * @note Time string format is always 24 characters long, 
 *       in format Ddd Mmm DD HH:MM:SS YYYY
 * 
 * @param str Time string
 * @param ttm Pointer to a tm struct
 * @return true Converted
 * @return false Something went wrong
 */

bool do_convtime(char *str, struct tm *ttm)
{
	char *buf = NULL, *p = NULL, *q = NULL;
	int i = 0;

	if (!str || !ttm)
	{
		return false;
	}

	while (*str == ' ')
	{
		str++;
	}

	buf = p = XMALLOC(SBUF_SIZE, "p");
	SAFE_SB_STR(str, buf, &p);
	*p = '\0';

	/** 
	 * day-of-week or month 
	 * 
	 */
	p = strchr(buf, ' ');
	if (p)
	{
		*p++ = '\0';
		while (*p == ' ')
			p++;
	}

	if (!p || strlen(buf) != 3)
	{
		XFREE(buf);
		return false;
	}

	for (i = 0; (i < 12) && string_compare(mdtab[i].month, p); i++)
		;

	if (i == 12)
	{
		/** 
		 * month 
		 * 
		 */
		q = strchr(p, ' ');
		if (q)
		{
			*q++ = '\0';
			while (*q == ' ')
				q++;
		}

		if (!q || strlen(p) != 3)
		{
			XFREE(buf);
			return false;
		}

		for (i = 0; (i < 12) && string_compare(mdtab[i].month, p); i++)
			;

		if (i == 12)
		{
			XFREE(buf);
			return false;
		}

		p = q;
	}

	ttm->tm_mon = i;

	/** 
	 * day of month 
	 * 
	 */
	q = strchr(p, ' ');
	if (q)
	{
		*q++ = '\0';
		while (*q == ' ')
			q++;
	}

	if (!q || (ttm->tm_mday = (int)strtol(p, (char **)NULL, 10)) < 1 || ttm->tm_mday > mdtab[i].day)
	{
		XFREE(buf);
		return false;
	}

	/** 
	 * hours 
	 * 
	 */
	p = strchr(q, ':');

	if (!p)
	{
		XFREE(buf);
		return false;
	}

	*p++ = '\0';

	if ((ttm->tm_hour = (int)strtol(q, (char **)NULL, 10)) > 23 || ttm->tm_hour < 0)
	{
		XFREE(buf);
		return false;
	}

	if (ttm->tm_hour == 0)
	{
		while (isspace(*q))
		{
			q++;
		}

		if (*q != '0')
		{
			XFREE(buf);
			return false;
		}
	}

	/** 
	 * minutes 
	 * 
	 */
	q = strchr(p, ':');

	if (!q)
	{
		XFREE(buf);
		return false;
	}

	*q++ = '\0';

	if ((ttm->tm_min = (int)strtol(p, (char **)NULL, 10)) > 59 || ttm->tm_min < 0)
	{
		XFREE(buf);
		return false;
	}

	if (ttm->tm_min == 0)
	{
		while (isspace(*p))
		{
			p++;
		}

		if (*p != '0')
		{
			XFREE(buf);
			return false;
		}
	}

	/** 
	 * seconds 
	 * 
	 */
	p = strchr(q, ' ');
	if (p)
	{
		*p++ = '\0';
		while (*p == ' ')
			p++;
	}

	if (!p || (ttm->tm_sec = (int)strtol(q, (char **)NULL, 10)) > 59 || ttm->tm_sec < 0)
	{
		XFREE(buf);
		return false;
	}

	if (ttm->tm_sec == 0)
	{
		while (isspace(*q))
		{
			q++;
		}

		if (*q != '0')
		{
			XFREE(buf);
			return false;
		}
	}

	/** 
	 * year 
	 * 
	 */
	q = strchr(p, ' ');
	if (q)
	{
		*q++ = '\0';
		while (*q == ' ')
			q++;
	}

	if ((ttm->tm_year = (int)strtol(p, (char **)NULL, 10)) == 0)
	{
		while (isspace(*p))
		{
			p++;
		}

		if (*p != '0')
		{
			XFREE(buf);
			return false;
		}
	}

	XFREE(buf);

	if (ttm->tm_year > 100)
	{
		ttm->tm_year -= 1900;
	}

	if (ttm->tm_year < 0)
	{
		return false;
	}

	/**
     * We don't whether or not it's daylight savings time.
	 * 
     */
	ttm->tm_isdst = -1;
	return (ttm->tm_mday != 29 || i != 1 || ((ttm->tm_year) % 400 == 100 || ((ttm->tm_year) % 100 != 0 && (ttm->tm_year) % 4 == 0))) ? true : false;
}

/**
 * @brief converts time string to seconds, based off 0:00 1/1/70
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player Not used
 * @param caller Not used
 * @param cause Not used
 * @param fargs Function's arguments
 * @param nfargs Not used
 * @param cargs Not used
 * @param ncargs Not used
 */
void fun_convtime(char *buff, char **bufc, dbref player __attribute__((unused)), dbref caller __attribute__((unused)), dbref cause __attribute__((unused)), char *fargs[], int nfargs __attribute__((unused)), char *cargs[] __attribute__((unused)), int ncargs __attribute__((unused)))
{
	struct tm *ttm = localtime(&mushstate.now);

	if (do_convtime(fargs[0], ttm))
	{
		SAFE_LTOS(buff, bufc, mktime(ttm), LBUF_SIZE);
	}
	else
	{
		SAFE_STRNCAT(buff, bufc, "-1", 2, LBUF_SIZE);
	}
}

/**
 * @brief Interface to strftime().
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player Not used
 * @param caller Not used
 * @param cause Not used
 * @param fargs Function's arguments
 * @param nfargs Number of function's arguments
 * @param cargs Not used
 * @param ncargs Not used
 */
void fun_timefmt(char *buff, char **bufc, dbref player __attribute__((unused)), dbref caller __attribute__((unused)), dbref cause __attribute__((unused)), char *fargs[], int nfargs, char *cargs[] __attribute__((unused)), int ncargs __attribute__((unused)))
{
	time_t tt = 0L;
	struct tm *ttm = NULL;
	char *str = XMALLOC(LBUF_SIZE, "str");
	char *tbuf = XMALLOC(LBUF_SIZE, "tbuf");
	char *tp = NULL, *p = NULL;
	int len = 0;

	/**
	 * Check number of arguments.
	 * 
	 */
	if ((nfargs < 1) || !fargs[0] || !*fargs[0])
	{
		XFREE(str);
		XFREE(tbuf);
		return;
	}

	if (nfargs == 1)
	{
		tt = mushstate.now;
	}
	else if (nfargs == 2)
	{
		tt = (time_t)strtol(fargs[1], (char **)NULL, 10);
		;

		if (tt < 0)
		{
			SAFE_LB_STR("#-1 INVALID TIME", buff, bufc);
			XFREE(str);
			XFREE(tbuf);
			return;
		}
	}
	else
	{
		SAFE_SPRINTF(buff, bufc, "#-1 FUNCTION (TIMEFMT) EXPECTS 1 OR 2 ARGUMENTS BUT GOT %d", nfargs);
		XFREE(str);
		XFREE(tbuf);
		return;
	}
	/**
	 * Construct the format string. We need to convert instances of '$'
	 * into percent signs for strftime(), unless we get a '$$', which we
	 * treat as a literal '$'. Step on '$n' as invalid (output literal
	 * '%n'), because some strftime()s use it to insert a newline.
	 * 
	 */
	for (tp = tbuf, p = fargs[0], len = 0; *p && (len < LBUF_SIZE - 2); tp++, p++)
	{
		if (*p == '%')
		{
			*tp++ = '%';
			*tp = '%';
		}
		else if (*p == '$')
		{
			if (*(p + 1) == '$')
			{
				*tp = '$';
				p++;
			}
			else if (*(p + 1) == 'n')
			{
				*tp++ = '%';
				*tp++ = '%';
				*tp = 'n';
				p++;
			}
			else
			{
				*tp = '%';
			}
		}
		else
		{
			*tp = *p;
		}
	}

	*tp = '\0';

	/**
	 * Get the time and format it. We do this using the local timezone.
	 * 
	 */
	ttm = localtime(&tt);
	strftime(str, LBUF_SIZE - 1, tbuf, ttm);
	SAFE_LB_STR(str, buff, bufc);
	XFREE(str);
	XFREE(tbuf);
}

/**
 * @brief Format a number of seconds into a human-readable time.
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player Not used
 * @param caller Not used
 * @param cause Not used
 * @param fargs Function's arguments
 * @param nfargs Not used
 * @param cargs Not used
 * @param ncargs Not used
 */
void fun_etimefmt(char *buff, char **bufc, dbref player __attribute__((unused)), dbref caller __attribute__((unused)), dbref cause __attribute__((unused)), char *fargs[], int nfargs __attribute__((unused)), char *cargs[] __attribute__((unused)), int ncargs __attribute__((unused)))
{
	char *p = NULL, *mark = NULL, padc = 0, timec = 0;
	int raw_secs = 0, secs = 0, mins = 0, hours = 0, days = 0, csecs = 0, cmins = 0, chours = 0, cdays = 0;
	int max = 0, n = 0, x = 0, width = 0, hidezero = 0, hideearly = 0, showsuffix = 0, clockfmt = 0, usecap = 0;

	/**
     * Figure out time values
	 * 
     */
	raw_secs = secs = (int)strtol(fargs[1], (char **)NULL, 10);

	if (secs < 0)
	{
		/**
		 * Try to be semi-useful. Keep value of secs; zero out the
		 * rest
		 * 
		 */
		mins = hours = days = 0;
	}
	else
	{
		days = secs / 86400;
		secs %= 86400;
		hours = secs / 3600;
		secs %= 3600;
		mins = secs / 60;
		secs %= 60;
	}

	/**
     * Parse and print format string
	 * 
     */
	p = fargs[0];

	while (*p)
	{
		if (*p == '$')
		{
			/** 
			 * save place in case we need to go back 
			 *
			 */
			mark = p;

			p++;

			if (!*p)
			{
				SAFE_LB_CHR('$', buff, bufc);
				break;
			}
			else if (*p == '$')
			{
				SAFE_LB_CHR('$', buff, bufc);
				p++;
			}
			else
			{
				hidezero = hideearly = showsuffix = clockfmt = usecap = 0;

				/**
				 * Optional width
				 * 
				 */
				for (width = 0; *p && isdigit((unsigned char)*p); p++)
				{
					width *= 10;
					width += *p - '0';
				}

				for (; (*p == 'z') || (*p == 'Z') || (*p == 'x') || (*p == 'X') || (*p == 'c') || (*p == 'C'); p++)
				{
					if (*p == 'z')
					{
						hidezero = 1;
					}
					else if (*p == 'Z')
					{
						hideearly = 1;
					}
					else if ((*p == 'x') || (*p == 'X'))
					{
						showsuffix = 1;
					}
					else if (*p == 'c')
					{
						clockfmt = 1;
					}
					else if (*p == 'C')
					{
						usecap = 1;
					}
				}

				switch (*p)
				{
				case 's':
				case 'S':
					if (usecap)
					{
						n = raw_secs;
					}
					else
					{
						n = secs;
					}

					timec = 's';
					break;

				case 'm':
				case 'M':
					if (usecap)
						n = mins + (hours * 60) + (days * 24 * 60);
					else
					{
						n = mins;
					}

					timec = 'm';
					break;

				case 'h':
				case 'H':
					if (usecap)
					{
						n = hours + (days * 24);
					}
					else
					{
						n = hours;
					}

					timec = 'h';
					break;

				case 'd':
				case 'D':
					n = days;
					timec = 'd';
					break;

				case 'a':
				case 'A':
					/**
				     * Show the first non-zero thing
					 * 
				     */
					if (days > 0)
					{
						n = days;
						timec = 'd';
					}
					else if (hours > 0)
					{
						n = hours;
						timec = 'h';
					}
					else if (mins > 0)
					{
						n = mins;
						timec = 'm';
					}
					else
					{
						n = secs;
						timec = 's';
					}

					break;

				default:
					timec = ' ';
				}

				if (timec == ' ')
				{
					while (*p && (*p != '$'))
					{
						p++;
					}

					SAFE_STRNCAT(buff, bufc, mark, p - mark, LBUF_SIZE);
				}
				else if (!clockfmt)
				{
					/**
				     * If it's 0 and we're hidezero, just
				     * hide it. If it's 0 and we're
				     * hideearly, we only hide it if we
		    		 * haven't got some bigger increment
				     * that's non-zero.
					 * 
				     */
					if ((n == 0) && (hidezero || (hideearly && !(((timec == 's') && (raw_secs > 0)) || ((timec == 'm') && (raw_secs >= 60)) || ((timec == 'h') && (raw_secs >= 3600))))))
					{
						if (width > 0)
						{
							padc = isupper(*p) ? '0' : ' ';

							if (showsuffix)
							{
								x = width + 1;

								if (x > 0)
								{
									max = LBUF_SIZE - 1 - (*bufc - buff);
									x = (x > max) ? max : x;
									XMEMSET(*bufc, padc, x);
									*bufc += x;
									**bufc = '\0';
								}
							}
							else
							{
								if (width > 0)
								{
									max = LBUF_SIZE - 1 - (*bufc - buff);
									width = (width > max) ? max : width;
									XMEMSET(*bufc, padc, width);
									*bufc += width;
									**bufc = '\0';
								}
							}
						}
					}
					else if (width > 0)
					{
						if (isupper(*p))
						{
							SAFE_SPRINTF(buff, bufc, "%0*d", width, n);
						}
						else
						{
							SAFE_SPRINTF(buff, bufc, "%*d", width, n);
						}

						if (showsuffix)
						{
							SAFE_LB_CHR(timec, buff, bufc);
						}
					}
					else
					{
						SAFE_LTOS(buff, bufc, n, LBUF_SIZE);

						if (showsuffix)
						{
							SAFE_LB_CHR(timec, buff, bufc);
						}
					}

					p++;
				}
				else
				{
					/*
				     * In clock format, we show
				     * <d>:<h>:<m>:<s>. The field
				     * specifier tells us where our
				     * division stops.
					 * 
				     */
					if (timec == 'd')
					{
						cdays = days;
						chours = hours;
						cmins = mins;
						csecs = secs;
					}
					else if (timec == 'h')
					{
						cdays = 0;
						csecs = raw_secs;
						chours = csecs / 3600;
						csecs %= 3600;
						cmins = csecs / 60;
						csecs %= 60;
					}
					else if (timec == 'm')
					{
						cdays = chours = 0;
						csecs = raw_secs;
						cmins = csecs / 60;
						csecs %= 60;
					}
					else
					{
						cdays = chours = cmins = 0;
						csecs = raw_secs;
					}

					if (!hidezero || (cdays != 0))
					{
						SAFE_SPRINTF(buff, bufc, isupper(*p) ? "%0*d:%0*d:%0*d:%0*d" : "%*d:%*d:%*d:%*d", width, cdays, width, chours, width, cmins, width, csecs);
					}
					else
					{
						/**
						 * Start from the first
						 * non-zero thing
						 * 
						 */
						if (chours != 0)
						{
							SAFE_SPRINTF(buff, bufc, isupper(*p) ? "%0*d:%0*d:%0*d" : "%*d:%*d:%*d", width, chours, width, cmins, width, csecs);
						}
						else if (cmins != 0)
						{
							SAFE_SPRINTF(buff, bufc, isupper(*p) ? "%0*d:%0*d" : "%*d:%*d", width, cmins, width, csecs);
						}
						else
						{
							SAFE_SPRINTF(buff, bufc, isupper(*p) ? "%0*d" : "%*d", width, csecs);
						}
					}

					p++;
				}
			}
		}
		else
		{
			mark = p;

			while (*p && (*p != '$'))
			{
				p++;
			}

			SAFE_STRNCAT(buff, bufc, mark, p - mark, LBUF_SIZE);
		}
	}
}

/**
 * @brief What time did this system last reboot?
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player Not used
 * @param caller Not used
 * @param cause Not used
 * @param fargs Not used
 * @param nfargs Not used
 * @param cargs Not used
 * @param ncargs Not used
 */
void fun_starttime(char *buff, char **bufc, dbref player __attribute__((unused)), dbref caller __attribute__((unused)), dbref cause __attribute__((unused)), char *fargs[] __attribute__((unused)), int nfargs __attribute__((unused)), char *cargs[] __attribute__((unused)), int ncargs __attribute__((unused)))
{
	char *temp = (char *)ctime(&mushstate.start_time);

	temp[strlen(temp) - 1] = '\0';
	SAFE_LB_STR(temp, buff, bufc);
}

/**
 * @brief How many times have we restarted?
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player Not used
 * @param caller Not used
 * @param cause Not used
 * @param fargs Not used
 * @param nfargs Not used
 * @param cargs Not used
 * @param ncargs Not used
 */
void fun_restarts(char *buff, char **bufc, dbref player __attribute__((unused)), dbref caller __attribute__((unused)), dbref cause __attribute__((unused)), char *fargs[] __attribute__((unused)), int nfargs __attribute__((unused)), char *cargs[] __attribute__((unused)), int ncargs __attribute__((unused)))
{
	SAFE_LTOS(buff, bufc, mushstate.reboot_nums, LBUF_SIZE);
}

/**
 * @brief When did we last restart?
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player Not used
 * @param caller Not used
 * @param cause Not used
 * @param fargs Not used
 * @param nfargs Not used
 * @param cargs Not used
 * @param ncargs Not used
 */
void fun_restarttime(char *buff, char **bufc, dbref player __attribute__((unused)), dbref caller __attribute__((unused)), dbref cause __attribute__((unused)), char *fargs[] __attribute__((unused)), int nfargs __attribute__((unused)), char *cargs[] __attribute__((unused)), int ncargs __attribute__((unused)))
{
	char *temp = (char *)ctime(&mushstate.restart_time);

	temp[strlen(temp) - 1] = '\0';
	SAFE_LB_STR(temp, buff, bufc);
}

/*
 * ---------------------------------------------------------------------------
 * fun_version: 
 */

/**
 * @brief Return the MUSH version.
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player Not used
 * @param caller Not used
 * @param cause Not used
 * @param fargs Not used
 * @param nfargs Not used
 * @param cargs Not used
 * @param ncargs Not used
 */
void fun_version(char *buff, char **bufc, dbref player __attribute__((unused)), dbref caller __attribute__((unused)), dbref cause __attribute__((unused)), char *fargs[] __attribute__((unused)), int nfargs __attribute__((unused)), char *cargs[] __attribute__((unused)), int ncargs __attribute__((unused)))
{
	char *s = XASPRINTF("%s [%s]", mushstate.version.name, PACKAGE_RELEASE_DATE);

	SAFE_LB_STR(s, buff, bufc);
	XFREE(s);
}

/**
 * @brief Return the name of the Mush.
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player Not used
 * @param caller Not used
 * @param cause Not used
 * @param fargs Not used
 * @param nfargs Not used
 * @param cargs Not used
 * @param ncargs Not used
 */
void fun_mushname(char *buff, char **bufc, dbref player __attribute__((unused)), dbref caller __attribute__((unused)), dbref cause __attribute__((unused)), char *fargs[] __attribute__((unused)), int nfargs __attribute__((unused)), char *cargs[] __attribute__((unused)), int ncargs __attribute__((unused)))
{
	SAFE_LB_STR(mushconf.mush_name, buff, bufc);
}

/**
 * @brief Return a list of modules.
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player Not used
 * @param caller Not used
 * @param cause Not used
 * @param fargs Function's arguments
 * @param nfargs Number of function's arguments
 * @param cargs Not used
 * @param ncargs Not used
 */
void fun_modules(char *buff, char **bufc, dbref player __attribute__((unused)), dbref caller __attribute__((unused)), dbref cause __attribute__((unused)), char *fargs[], int nfargs, char *cargs[] __attribute__((unused)), int ncargs __attribute__((unused)))
{
	MODULE *mp = NULL;
	bool got_one = false;

	for (mp = mushstate.modules_list; mp != NULL; mp = mp->next)
	{
		if (got_one)
		{
			if (nfargs >= 1)
			{
				if (fargs[0] || *fargs[0])
				{
					SAFE_LB_STR(fargs[0], buff, bufc);
				}
			}
			else
			{
				SAFE_LB_CHR(' ', buff, bufc);
			}
		}
		SAFE_LB_STR(mp->modname, buff, bufc);
		got_one = true;
	}
}

/**
 * @brief Return 1 if a module is installed, 0 if it is not.
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player Not used
 * @param caller Not used
 * @param cause Not used
 * @param fargs Function's arguments
 * @param nfargs Not used
 * @param cargs Not used
 * @param ncargs Not used
 */
void fun_hasmodule(char *buff, char **bufc, dbref player __attribute__((unused)), dbref caller __attribute__((unused)), dbref cause __attribute__((unused)), char *fargs[], int nfargs __attribute__((unused)), char *cargs[] __attribute__((unused)), int ncargs __attribute__((unused)))
{
	MODULE *mp = NULL;

	for (mp = mushstate.modules_list; mp != NULL; mp = mp->next)
	{
		if (!strcasecmp(fargs[0], mp->modname))
		{
			SAFE_LB_CHR('1', buff, bufc);
			return;
		}
	}

	SAFE_LB_CHR('0', buff, bufc);
}

/**
 * @brief Get max number of simultaneous connects.
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player Not used
 * @param caller Not used
 * @param cause Not used
 * @param fargs Not used
 * @param nfargs Not used
 * @param cargs Not used
 * @param ncargs Not used
 */
void fun_connrecord(char *buff, char **bufc, dbref player __attribute__((unused)), dbref caller __attribute__((unused)), dbref cause __attribute__((unused)), char *fargs[] __attribute__((unused)), int nfargs __attribute__((unused)), char *cargs[] __attribute__((unused)), int ncargs __attribute__((unused)))
{
	SAFE_LTOS(buff, bufc, mushstate.record_players, LBUF_SIZE);
}

/**
 * @brief State of the function invocation counters
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player Not used
 * @param caller Not used
 * @param cause Not used
 * @param fargs Not used
 * @param nfargs Not used
 * @param cargs Not used
 * @param ncargs Not used
 */
void fun_fcount(char *buff, char **bufc, dbref player __attribute__((unused)), dbref caller __attribute__((unused)), dbref cause __attribute__((unused)), char *fargs[] __attribute__((unused)), int nfargs __attribute__((unused)), char *cargs[] __attribute__((unused)), int ncargs __attribute__((unused)))
{
	SAFE_LTOS(buff, bufc, mushstate.func_invk_ctr, LBUF_SIZE);
}

/**
 * @brief State of the function recursion counters
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player Not used
 * @param caller Not used
 * @param cause Not used
 * @param fargs Not used
 * @param nfargs Not used
 * @param cargs Not used
 * @param ncargs Not used
 */
void fun_fdepth(char *buff, char **bufc, dbref player __attribute__((unused)), dbref caller __attribute__((unused)), dbref cause __attribute__((unused)), char *fargs[] __attribute__((unused)), int nfargs __attribute__((unused)), char *cargs[] __attribute__((unused)), int ncargs __attribute__((unused)))
{
	SAFE_LTOS(buff, bufc, mushstate.func_nest_lev, LBUF_SIZE);
}

/**
 * @brief State of the command invocation counters
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player Not used
 * @param caller Not used
 * @param cause Not used
 * @param fargs Not used
 * @param nfargs Not used
 * @param cargs Not used
 * @param ncargs Not used
 */
void fun_ccount(char *buff, char **bufc, dbref player __attribute__((unused)), dbref caller __attribute__((unused)), dbref cause __attribute__((unused)), char *fargs[] __attribute__((unused)), int nfargs __attribute__((unused)), char *cargs[] __attribute__((unused)), int ncargs __attribute__((unused)))
{
	SAFE_LTOS(buff, bufc, mushstate.cmd_invk_ctr, LBUF_SIZE);
}

/**
 * @brief State of the command recursion counters
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player Not used
 * @param caller Not used
 * @param cause Not used
 * @param fargs Not used
 * @param nfargs Not used
 * @param cargs Not used
 * @param ncargs Not used
 */
void fun_cdepth(char *buff, char **bufc, dbref player __attribute__((unused)), dbref caller __attribute__((unused)), dbref cause __attribute__((unused)), char *fargs[] __attribute__((unused)), int nfargs __attribute__((unused)), char *cargs[] __attribute__((unused)), int ncargs __attribute__((unused)))
{
	SAFE_LTOS(buff, bufc, mushstate.cmd_nest_lev, LBUF_SIZE);
}

/**
 * @brief Benchmark softcode.
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player DBref of player
 * @param caller DBref of caller
 * @param cause DBref of cause
 * @param fargs Function's arguments
 * @param nfargs Not used
 * @param cargs Command's arguments
 * @param ncargs Nomber of command's arguments
 */
void fun_benchmark(char *buff, char **bufc, dbref player, dbref caller, dbref cause, char *fargs[], int nfargs __attribute__((unused)), char *cargs[], int ncargs)
{
	struct timeval bt, et;
	int i = 0, times = 0;
	double min = 0.0, max = 0.0, total = 0.0, ut = 0.0;
	char *tp = NULL, *nstr = NULL, *s = NULL;
	char *ebuf = XMALLOC(LBUF_SIZE, "ebuf");
	char *tbuf = XMALLOC(LBUF_SIZE, "ebuf");

	/**
     * Evaluate our times argument
	 * 
     */
	tp = nstr = XMALLOC(LBUF_SIZE, "nstr");
	s = fargs[1];
	exec(nstr, &tp, player, caller, cause, EV_EVAL | EV_STRIP | EV_FCHECK, &s, cargs, ncargs);
	times = (int)strtol(nstr, (char **)NULL, 10);
	XFREE(nstr);

	if (times < 1)
	{
		SAFE_LB_STR("#-1 TOO FEW TIMES", buff, bufc);
		XFREE(ebuf);
		XFREE(tbuf);
		return;
	}

	if (times > mushconf.func_invk_lim)
	{
		SAFE_LB_STR("#-1 TOO MANY TIMES", buff, bufc);
		XFREE(ebuf);
		XFREE(tbuf);
		return;
	}

	min = max = total = 0;

	for (i = 0; i < times; i++)
	{
		XSTRCPY(ebuf, fargs[0]);
		s = ebuf;
		tp = tbuf;

		gettimeofday(&bt, NULL);

		exec(tbuf, &tp, player, caller, cause, EV_FCHECK | EV_STRIP | EV_EVAL, &s, cargs, ncargs);

		gettimeofday(&et, NULL);

		ut = ((et.tv_sec - bt.tv_sec) * 1000000) + (et.tv_usec - bt.tv_usec);

		if ((ut < min) || (min == 0))
		{
			min = ut;
		}

		if (ut > max)
		{
			max = ut;
		}

		total += ut;

		if ((mushstate.func_invk_ctr >= mushconf.func_invk_lim) || (Too_Much_CPU()))
		{
			/**
		     * Abort
			 * 
		     */
			notify_check(player, player, MSG_PUP_ALWAYS | MSG_ME_ALL | MSG_F_DOWN, "Limits exceeded at benchmark iteration %d.", i + 1);
			times = i + 1;
		}
	}

	SAFE_SPRINTF(buff, bufc, "%.2f %.0f %.0f", total / (double)times, min, max);
	XFREE(ebuf);
	XFREE(tbuf);
}

/*
 * ---------------------------------------------------------------------------
 * fun_s: Force substitution to occur. fun_subeval: Like s(), but don't do
 * function evaluations.
 */

/**
 * @brief Force substitution to occur.
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player DBref of player
 * @param caller DBref of caller
 * @param cause DBref of cause
 * @param fargs Function's arguments
 * @param nfargs Not used
 * @param cargs Command's arguments
 * @param ncargs Nomber of command's arguments
 */
void fun_s(char *buff, char **bufc, dbref player, dbref caller, dbref cause, char *fargs[], int nfargs __attribute__((unused)), char *cargs[], int ncargs)
{
	char *str;
	str = fargs[0];
	exec(buff, bufc, player, caller, cause, EV_FIGNORE | EV_EVAL, &str, cargs, ncargs);
}

/**
 * @brief Like s(), but don't do function evaluations.
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player DBref of player
 * @param caller DBref of caller
 * @param cause DBref of cause
 * @param fargs Function's arguments
 * @param nfargs Not used
 * @param cargs Not used
 * @param ncargs Not used
 */
void fun_subeval(char *buff, char **bufc, dbref player, dbref caller, dbref cause, char *fargs[], int nfargs __attribute__((unused)), char *cargs[] __attribute__((unused)), int ncargs __attribute__((unused)))
{
	char *str;
	str = fargs[0];
	exec(buff, bufc, player, caller, cause, EV_NO_LOCATION | EV_NOFCHECK | EV_FIGNORE | EV_NO_COMPRESS, &str, (char **)NULL, 0);
}

/*------------------------------------------------------------------------
 * Side-effect functions.
 */

/**
 * @brief Check if the player can execute a command
 * 
 * @param player DBref of player
 * @param name Name of command
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param cargs Command's arguments
 * @param ncargs Nomber of command's arguments
 * @return true User can execute the command
 * @return false User is not allowed to used the command
 */

bool check_command(dbref player, char *name, char *buff, char **bufc, char *cargs[], int ncargs)
{
	CMDENT *cmdp;

	if ((cmdp = (CMDENT *)hashfind(name, &mushstate.command_htab)))
	{
		/**
		 * Note that these permission checks are NOT identical to the
		 * ones in process_cmdent(). In particular, side-effects are
		 * NOT subject to the CA_GBL_INTERP flag. This is a design
		 * decision based on the concept that these are functions and
		 * not commands, even though they behave like commands in
		 * many respects. This is also the same reason why
		 * side-effects don't trigger hooks.
		 * 
		 */
		if (Invalid_Objtype(player) || !check_cmd_access(player, cmdp, cargs, ncargs) || (!Builder(player) && Protect(CA_GBL_BUILD) && !(mushconf.control_flags & CF_BUILD)))
		{
			SAFE_NOPERM(buff, bufc);
			return false;
		}
	}

	return true;
}

/**
 * @brief This side-effect function links an object to a location,
 *        behaving identically to the command:
 *        '@link <object>=<destination>'
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player DBref of player
 * @param caller Not used
 * @param cause DBref of cause
 * @param fargs Function's arguments
 * @param nfargs Not used
 * @param cargs Command's arguments
 * @param ncargs Nomber of command's arguments
 */
void fun_link(char *buff, char **bufc, dbref player, dbref caller __attribute__((unused)), dbref cause, char *fargs[], int nfargs __attribute__((unused)), char *cargs[], int ncargs)
{
	if (!check_command(player, "@link", buff, bufc, cargs, ncargs))
	{
		return;
	}

	do_link(player, cause, 0, fargs[0], fargs[1]);
}

/**
 * @brief This side-effect function teleports an object from one place to 
 *        another, behaving identically to the command:
 *        '@tel <object>=<destination>'
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player DBref of player
 * @param caller Not used
 * @param cause DBref of cause
 * @param fargs Function's arguments
 * @param nfargs Not used
 * @param cargs Command's arguments
 * @param ncargs Nomber of command's arguments
 */
void fun_tel(char *buff, char **bufc, dbref player, dbref caller __attribute__((unused)), dbref cause, char *fargs[], int nfargs __attribute__((unused)), char *cargs[], int ncargs)
{
	if (!check_command(player, "@teleport", buff, bufc, cargs, ncargs))
	{
		return;
	}

	do_teleport(player, cause, 0, fargs[0], fargs[1]);
}

/**
 * @brief This side-effect function erases some or all attributes from an
 *        object, behaving identically to the command:
 *        '@wipe <object>[</wild-attr>]'
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player DBref of player
 * @param caller Not used
 * @param cause DBref of cause
 * @param fargs Function's arguments
 * @param nfargs Not used
 * @param cargs Command's arguments
 * @param ncargs Nomber of command's arguments
 */
void fun_wipe(char *buff, char **bufc, dbref player, dbref caller __attribute__((unused)), dbref cause, char *fargs[], int nfargs __attribute__((unused)), char *cargs[], int ncargs)
{
	if (!check_command(player, "@wipe", buff, bufc, cargs, ncargs))
	{
		return;
	}

	do_wipe(player, cause, 0, fargs[0]);
}

/**
 * @brief This side-effect function sends a message to the list of dbrefs,
 *        behaving identically to the command:
 *        '@pemit/list <list of dbrefs>=<string>'
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player DBref of player
 * @param caller Not used
 * @param cause Not used
 * @param fargs Function's arguments
 * @param nfargs Not used
 * @param cargs Command's arguments
 * @param ncargs Nomber of command's arguments
 */
void fun_pemit(char *buff, char **bufc, dbref player, dbref caller __attribute__((unused)), dbref cause __attribute__((unused)), char *fargs[], int nfargs __attribute__((unused)), char *cargs[], int ncargs)
{
	if (!check_command(player, "@pemit", buff, bufc, cargs, ncargs))
	{
		return;
	}

	do_pemit_list(player, fargs[0], fargs[1], 0);
}

/**
 * @brief This side-effect function sends a message to the list of dbrefs, 
 *        behaving identically to the command:
 *        '@pemit/list/contents <list of dbrefs>=<string>'
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player DBref of player
 * @param caller Not used
 * @param cause Not used
 * @param fargs Function's arguments
 * @param nfargs Not used
 * @param cargs Command's arguments
 * @param ncargs Nomber of command's arguments
 */
void fun_remit(char *buff, char **bufc, dbref player, dbref caller __attribute__((unused)), dbref cause __attribute__((unused)), char *fargs[], int nfargs __attribute__((unused)), char *cargs[], int ncargs)
{
	if (!check_command(player, "@pemit", buff, bufc, cargs, ncargs))
	{
		return;
	}

	do_pemit_list(player, fargs[0], fargs[1], 1);
}

/**
 * @brief This side-effect function sends a message to everyone in <target>'s
 *        location with the exception of <target>, 
 *        behaving identically to the command:
 *        '@oemit <target> = <string>'
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player DBref of player
 * @param caller Not used
 * @param cause DBref of cause
 * @param fargs Function's arguments
 * @param nfargs Not used
 * @param cargs Command's arguments
 * @param ncargs Nomber of command's arguments
 */
void fun_oemit(char *buff, char **bufc, dbref player, dbref caller __attribute__((unused)), dbref cause, char *fargs[], int nfargs __attribute__((unused)), char *cargs[], int ncargs)
{
	if (!check_command(player, "@oemit", buff, bufc, cargs, ncargs))
	{
		return;
	}

	do_pemit(player, cause, PEMIT_OEMIT, fargs[0], fargs[1]);
}

/**
 * @brief This side-effect function behaves identically to the command
 *        '@force <object>=<action>'
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player DBref of player
 * @param caller Not used
 * @param cause DBref of cause
 * @param fargs Function's arguments
 * @param nfargs Not used
 * @param cargs Command's arguments
 * @param ncargs Nomber of command's arguments
 */
void fun_force(char *buff, char **bufc, dbref player, dbref caller __attribute__((unused)), dbref cause, char *fargs[], int nfargs __attribute__((unused)), char *cargs[], int ncargs)
{
	if (!check_command(player, "@force", buff, bufc, cargs, ncargs))
	{
		return;
	}

	do_force(player, cause, FRC_NOW, fargs[0], fargs[1], cargs, ncargs);
}

/**
 * @brief This side-effect function behaves identically to the command
 *        '@trigger <object>/<attribute>=<arg 0>,<arg 1>,...<arg N>'
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player DBref of player
 * @param caller Not used
 * @param cause DBref of cause
 * @param fargs Function's arguments
 * @param nfargs Number of function's arguments
 * @param cargs Command's arguments
 * @param ncargs Nomber of command's arguments
 */
void fun_trigger(char *buff, char **bufc, dbref player, dbref caller __attribute__((unused)), dbref cause, char *fargs[], int nfargs, char *cargs[], int ncargs)
{
	if (nfargs < 1)
	{
		SAFE_LB_STR("#-1 TOO FEW ARGUMENTS", buff, bufc);
		return;
	}

	if (!check_command(player, "@trigger", buff, bufc, cargs, ncargs))
	{
		return;
	}

	do_trigger(player, cause, TRIG_NOW, fargs[0], &(fargs[1]), nfargs - 1);
}

/**
 * @brief This side-effect function behaves identically to the command
 *        '@wait <timer>=<command>'
 * 
 * @param buff Not used
 * @param bufc Not used
 * @param player DBref of player
 * @param caller Not used
 * @param cause DBref of cause
 * @param fargs Function's arguments
 * @param nfargs Not used
 * @param cargs Command's arguments
 * @param ncargs Nomber of command's arguments
 */
void fun_wait(char *buff __attribute__((unused)), char **bufc __attribute__((unused)), dbref player, dbref caller __attribute__((unused)), dbref cause, char *fargs[], int nfargs __attribute__((unused)), char *cargs[], int ncargs)
{
	do_wait(player, cause, 0, fargs[0], fargs[1], cargs, ncargs);
}

/**
 * @brief This function executes <command> with the given arguments. 
 *        <command> is presently limited to @chown, @clone, @destroy, 
 *        @link, @lock, @name, @parent, @teleport, @unlink, @unlock, 
 *        and @wipe.
 * 
 * @param buff Not used
 * @param bufc Not used
 * @param player DBref of player
 * @param caller Not used
 * @param cause DBref of cause
 * @param fargs Function's arguments
 * @param nfargs Not used
 * @param cargs Command's arguments
 * @param ncargs Nomber of command's arguments
 */
void fun_command(char *buff __attribute__((unused)), char **bufc __attribute__((unused)), dbref player, dbref caller __attribute__((unused)), dbref cause, char *fargs[], int nfargs __attribute__((unused)), char *cargs[], int ncargs)
{
	CMDENT *cmdp = NULL;
	char *tbuf1 = NULL, *tbuf2 = NULL, *p = NULL;
	int key = 0;

	if (!fargs[0] || !*fargs[0])
	{
		return;
	}

	for (p = fargs[0]; *p; p++)
	{
		*p = tolower(*p);
	}

	cmdp = (CMDENT *)hashfind(fargs[0], &mushstate.command_htab);

	if (!cmdp)
	{
		notify(player, "Command not found.");
		return;
	}

	if (Invalid_Objtype(player) || !check_cmd_access(player, cmdp, cargs, ncargs) || (!Builder(player) && Protect(CA_GBL_BUILD) && !(mushconf.control_flags & CF_BUILD)))
	{
		notify(player, NOPERM_MESSAGE);
		return;
	}

	if (!(cmdp->callseq & CS_FUNCTION) || (cmdp->callseq & CS_ADDED))
	{
		notify(player, "Cannot call that command.");
		return;
	}

	/**
     * Strip command flags that are irrelevant.
	 * 
     */
	key = cmdp->extra;
	key &= ~(SW_GOT_UNIQUE | SW_MULTIPLE | SW_NOEVAL);
	switch (cmdp->callseq & CS_NARG_MASK)
	{
	case CS_NO_ARGS:
		(*(cmdp->info.handler))(player, cause, key);
		break;

	case CS_ONE_ARG:
		tbuf1 = XMALLOC(1, "tbuf1");
		(*(cmdp->info.handler))(player, cause, key, ((fargs[1]) ? (fargs[1]) : tbuf1));
		XFREE(tbuf1);
		break;

	case CS_TWO_ARG:
		tbuf1 = XMALLOC(1, "tbuf1");
		tbuf2 = XMALLOC(1, "tbuf2");
		(*(cmdp->info.handler))(player, cause, key, ((fargs[1]) ? (fargs[1]) : tbuf1), ((fargs[2]) ? (fargs[2]) : tbuf2));
		XFREE(tbuf2);
		XFREE(tbuf1);
		break;

	default:
		notify(player, "Invalid command handler.");
		return;
	}
}

/**
 * @brief Creates a room, thing or exit
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player DBref of player
 * @param caller DBref of caller
 * @param cause DBref of cause
 * @param fargs Function's arguments
 * @param nfargs Not used
 * @param cargs Command's arguments
 * @param ncargs Nomber of command's arguments
 */
void fun_create(char *buff, char **bufc, dbref player, dbref caller, dbref cause, char *fargs[], int nfargs, char *cargs[], int ncargs)
{
	dbref thing;
	int cost;
	char *name;
	Delim isep;

	if (!fn_range_check(((FUN *)fargs[-1])->name, nfargs, 2, 3, buff, bufc))
	{
		return;
	}

	if (!delim_check(buff, bufc, player, caller, cause, fargs, nfargs, cargs, ncargs, 3, &isep, 0))
	{
		return;
	}

	name = fargs[0];

	if (!name || !*name)
	{
		SAFE_LB_STR("#-1 ILLEGAL NAME", buff, bufc);
		return;
	}

	switch (isep.str[0])
	{
	case 'r':
		if (!check_command(player, "@dig", buff, bufc, cargs, ncargs))
		{
			return;
		}

		thing = create_obj(player, TYPE_ROOM, name, 0);
		break;

	case 'e':
		if (!check_command(player, "@open", buff, bufc, cargs, ncargs))
		{
			return;
		}

		thing = create_obj(player, TYPE_EXIT, name, 0);

		if (thing != NOTHING)
		{
			s_Exits(thing, player);
			s_Next(thing, Exits(player));
			s_Exits(player, thing);
		}

		break;

	default:
		if (!check_command(player, "@create", buff, bufc, cargs, ncargs))
		{
			return;
		}

		if (fargs[1] && *fargs[1])
		{
			cost = (int)strtol(fargs[1], (char **)NULL, 10);

			if (cost < mushconf.createmin || cost > mushconf.createmax)
			{
				SAFE_LB_STR("#-1 COST OUT OF RANGE", buff, bufc);
				return;
			}
		}
		else
		{
			cost = mushconf.createmin;
		}

		thing = create_obj(player, TYPE_THING, name, cost);

		if (thing != NOTHING)
		{
			move_via_generic(thing, player, NOTHING, 0);
			s_Home(thing, new_home(player));
		}

		break;
	}

	SAFE_LB_CHR('#', buff, bufc);
	SAFE_LTOS(buff, bufc, thing, LBUF_SIZE);
}

/*---------------------------------------------------------------------------
 * fun_set: 
 */

/**
 * @brief Sets an attribute on an object
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player DBref of player
 * @param caller Not used
 * @param cause Not used
 * @param fargs Function's arguments
 * @param nfargs Not used
 * @param cargs Command's arguments
 * @param ncargs Nomber of command's arguments
 */
void fun_set(char *buff, char **bufc, dbref player, dbref caller __attribute__((unused)), dbref cause __attribute__((unused)), char *fargs[], int nfargs __attribute__((unused)), char *cargs[], int ncargs)
{
	dbref thing, thing2, aowner;
	char *p, *buff2;
	int atr, atr2, aflags, alen, clear, flagvalue;
	ATTR *attr, *attr2;

	/**
     * obj/attr form?
	 * 
     */
	if (!check_command(player, "@set", buff, bufc, cargs, ncargs))
	{
		return;
	}

	if (parse_attrib(player, fargs[0], &thing, &atr, 0))
	{
		if (atr != NOTHING)
		{
			/**
		     * must specify flag name
			 * 
		     */
			if (!fargs[1] || !*fargs[1])
			{
				SAFE_LB_STR("#-1 UNSPECIFIED PARAMETER", buff, bufc);
			}

			/**
		     * are we clearing?
			 * 
		     */
			clear = 0;
			p = fargs[1];

			if (*fargs[1] == NOT_TOKEN)
			{
				p++;
				clear = 1;
			}

			/**
		     * valid attribute flag?
			 * 
		     */
			flagvalue = search_nametab(player, indiv_attraccess_nametab, p);

			if (flagvalue < 0)
			{
				SAFE_LB_STR("#-1 CAN NOT SET", buff, bufc);
				return;
			}

			/**
		     * make sure attribute is present
			 * 
		     */
			if (!atr_get_info(thing, atr, &aowner, &aflags))
			{
				SAFE_LB_STR("#-1 ATTRIBUTE NOT PRESENT ON OBJECT", buff, bufc);
				return;
			}

			/**
		     * can we write to attribute?
			 * 
		     */
			attr = atr_num(atr);

			if (!attr || !Set_attr(player, thing, attr, aflags))
			{
				SAFE_NOPERM(buff, bufc);
				return;
			}

			/**
		     * just do it!
			 * 
		     */
			if (clear)
			{
				aflags &= ~flagvalue;
			}
			else
			{
				aflags |= flagvalue;
			}

			Hearer(thing);
			atr_set_flags(thing, atr, aflags);
			return;
		}
	}

	/**
     * find thing
	 * 
     */
	if ((thing = match_controlled(player, fargs[0])) == NOTHING)
	{
		SAFE_NOTHING(buff, bufc);
		return;
	}

	/**
     * check for attr set first
	 * 
     */
	for (p = fargs[1]; *p && (*p != ':'); p++)
		;

	if (*p)
	{
		*p++ = 0;
		atr = mkattr(fargs[1]);

		if (atr <= 0)
		{
			SAFE_LB_STR("#-1 UNABLE TO CREATE ATTRIBUTE", buff, bufc);
			return;
		}

		attr = atr_num(atr);

		if (!attr)
		{
			SAFE_NOPERM(buff, bufc);
			return;
		}

		atr_get_info(thing, atr, &aowner, &aflags);

		if (!Set_attr(player, thing, attr, aflags))
		{
			SAFE_NOPERM(buff, bufc);
			return;
		}

		buff2 = XMALLOC(LBUF_SIZE, "buff2");

		/**
		 * check for _
		 * 
		 */
		if (*p == '_')
		{
			XSTRCPY(buff2, p + 1);

			if (!parse_attrib(player, p + 1, &thing2, &atr2, 0) || (atr == NOTHING))
			{
				XFREE(buff2);
				SAFE_NOMATCH(buff, bufc);
				return;
			}

			attr2 = atr_num(atr);
			p = buff2;
			atr_pget_str(buff2, thing2, atr2, &aowner, &aflags, &alen);

			if (!attr2 || !See_attr(player, thing2, attr2, aowner, aflags))
			{
				XFREE(buff2);
				SAFE_NOPERM(buff, bufc);
				return;
			}
		}

		/**
		 * set it
		 * 
		 */
		set_attr_internal(player, thing, atr, p, 0, buff, bufc);
		XFREE(buff2);
		return;
	}

	/**
     * set/clear a flag
	 * 
     */
	flag_set(thing, player, fargs[1], 0);
}

/*---------------------------------------------------------------------------
 * fun_ps: Gets details about the queue.
 *   ps(): Lists everything on the queue by PID
 *   ps(<object or player>): Lists PIDs enqueued by object or player's stuff
 *   ps(<PID>): Results in '<PID>:<wait status> <command>'
 */

/**
 * @brief List the queue pids
 * 
 * @param player_targ DBref of target player
 * @param obj_targ DBref of target object
 * @param queue Queue
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param bb_p Original value of output buffer tracher
 */
void list_qpids(dbref player_targ, dbref obj_targ, BQUE *queue, char *buff, char **bufc, char *bb_p)
{
	BQUE *tmp;

	for (tmp = queue; tmp; tmp = tmp->next)
	{
		if (que_want(tmp, player_targ, obj_targ))
		{
			if (*bufc != bb_p)
			{
				print_separator(&SPACE_DELIM, buff, bufc);
			}

			SAFE_LTOS(buff, bufc, tmp->pid, LBUF_SIZE);
		}
	}
}

/**
 * @brief Gets details about the queue.
 *        ps(): Lists everything on the queue by PID
 *        ps(<object or player>): Lists PIDs enqueued by object or player's stuff
 *        ps(<PID>): Results in '<PID>:<wait status> <command>'
 * 
 * @param buff Output buffer
 * @param bufc Output buffer tracker
 * @param player DBref of player
 * @param caller Not used
 * @param cause Not used
 * @param fargs Function's arguments
 * @param nfargs Not used
 * @param cargs Not used
 * @param ncargs Not used
 */
void fun_ps(char *buff, char **bufc, dbref player, dbref caller __attribute__((unused)), dbref cause __attribute__((unused)), char *fargs[], int nfargs __attribute__((unused)), char *cargs[] __attribute__((unused)), int ncargs __attribute__((unused)))
{
	int qpid;
	dbref player_targ, obj_targ;
	BQUE *qptr;
	ATTR *ap;
	char *bb_p;

	/**
     * Check for the PID case first.
	 * 
     */
	if (fargs[0] && is_integer(fargs[0]))
	{
		qpid = (int)strtol(fargs[0], (char **)NULL, 10);
		qptr = (BQUE *)nhashfind(qpid, &mushstate.qpid_htab);

		if (qptr == NULL)
		{
			return;
		}

		if ((qptr->waittime > 0) && (Good_obj(qptr->sem)))
		{
			SAFE_SPRINTF(buff, bufc, "#%d:#%d/%d %s", qptr->player, qptr->sem, qptr->waittime - mushstate.now, qptr->comm);
		}
		else if (qptr->waittime > 0)
		{
			SAFE_SPRINTF(buff, bufc, "#%d:%d %s", qptr->player, qptr->waittime - mushstate.now, qptr->comm);
		}
		else if (Good_obj(qptr->sem))
		{
			if (qptr->attr == A_SEMAPHORE)
			{
				SAFE_SPRINTF(buff, bufc, "#%d:#%d %s", qptr->player, qptr->sem, qptr->comm);
			}
			else
			{
				ap = atr_num(qptr->attr);

				if (ap && ap->name)
				{
					SAFE_SPRINTF(buff, bufc, "#%d:#%d/%s %s", qptr->player, qptr->sem, ap->name, qptr->comm);
				}
				else
				{
					SAFE_SPRINTF(buff, bufc, "#%d:#%d %s", qptr->player, qptr->sem, qptr->comm);
				}
			}
		}
		else
		{
			SAFE_SPRINTF(buff, bufc, "#%d: %s", qptr->player, qptr->comm);
		}

		return;
	}

	/**
     * We either have nothing specified, or an object or player.
	 * 
     */
	if (!fargs[0] || !*fargs[0])
	{
		if (!See_Queue(player))
		{
			return;
		}

		obj_targ = NOTHING;
		player_targ = NOTHING;
	}
	else
	{
		player_targ = Owner(player);

		if (See_Queue(player))
		{
			obj_targ = match_thing(player, fargs[0]);
		}
		else
		{
			obj_targ = match_controlled(player, fargs[0]);
		}

		if (!Good_obj(obj_targ))
		{
			return;
		}

		if (isPlayer(obj_targ))
		{
			player_targ = obj_targ;
			obj_targ = NOTHING;
		}
	}

	/**
     * List all the PIDs that match.
	 * 
     */
	bb_p = *bufc;
	list_qpids(player_targ, obj_targ, mushstate.qfirst, buff, bufc, bb_p);
	list_qpids(player_targ, obj_targ, mushstate.qlfirst, buff, bufc, bb_p);
	list_qpids(player_targ, obj_targ, mushstate.qwait, buff, bufc, bb_p);
	list_qpids(player_targ, obj_targ, mushstate.qsemfirst, buff, bufc, bb_p);
}
