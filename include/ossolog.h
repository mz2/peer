/*
 *  ossolog.h
 *  peer
 *
 *  Created by Matias Piipari on 21/12/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

/**
 @file osso-log.h
 The OSSO logging macros. Version: 20 Mar 2005.
 <p>
 Macro definitions that have an effect to the logging macros:
 
 DEBUG, OSSOLOG_COMPILE    Whether to compile the macros
 (prefer OSSOLOG_COMPILE over DEBUG)
 OSSOLOG_STDOUT            All logging to stdout(*
 OSSOLOG_STDERR            All logging to stderr(*
 
 *) No timestamp nor file name.
 
 To see the logging output of D-Bus-activated programs, define
 DBUS_DEBUG_OUTPUT environment variable for the D-Bus daemon and
 define the OSSOLOG_STDERR macro at compile time.
 
 Note that these macros are provided because Glib logging does not
 currently allow stripping the logging function calls away at
 compile time.
 
 ---
 
 This file is part of libosso.
 
 Copyright (C) 2004-2005 Nokia Corporation. All rights reserved.
 
 Contact: Kimmo H�m�l�inen <kimmo.hamalainen@nokia.com>
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public License
 version 2.1 as published by the Free Software Foundation.
 
 This library is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 02110-1301 USA
 */

#ifndef OSSO_LOG_H
#define OSSO_LOG_H

#ifdef DEBUG
#define OSSOLOG_COMPILE
#endif

#ifdef OSSOLOG_COMPILE
#if defined OSSOLOG_STDOUT || defined OSSOLOG_STDERR
#include <stdio.h> /* printf(), fprintf() */
#else
#include <syslog.h> /* syslog(), openlog(), closelog() */
#endif
#endif

/*** the macros ***/

#ifdef OSSOLOG_COMPILE

/* the binary has logging compiled in */

#if !defined OSSOLOG_STDOUT && !defined OSSOLOG_STDERR

/* log to the syslog */

#define ULOG_CRIT(...) syslog(LOG_CRIT | LOG_USER, __VA_ARGS__)
#define ULOG_ERR(...) syslog(LOG_ERR | LOG_USER, __VA_ARGS__)
#define ULOG_WARN(...) syslog(LOG_WARNING | LOG_USER, __VA_ARGS__)
#define ULOG_INFO(...) syslog(LOG_INFO | LOG_USER, __VA_ARGS__)
#define ULOG_DEBUG(...) syslog(LOG_DEBUG | LOG_USER, __VA_ARGS__)
#define ULOG_CRIT_L(FMT, ARG...) syslog(LOG_CRIT | LOG_USER, \
"%s:%d: " FMT, __FILE__, __LINE__, ## ARG)
#define ULOG_ERR_L(FMT, ARG...) syslog(LOG_ERR | LOG_USER, \
"%s:%d: " FMT, __FILE__, __LINE__, ## ARG)
#define ULOG_WARN_L(FMT, ARG...) syslog(LOG_WARNING | LOG_USER, \
"%s:%d: " FMT, __FILE__, __LINE__, ## ARG)
#define ULOG_INFO_L(FMT, ARG...) syslog(LOG_INFO | LOG_USER, \
"%s:%d: " FMT, __FILE__, __LINE__, ## ARG)
#define ULOG_DEBUG_L(FMT, ARG...) syslog(LOG_DEBUG | LOG_USER, \
"%s:%d: " FMT, __FILE__, __LINE__, ## ARG)
#define ULOG_CRIT_F(FMT, ARG...) syslog(LOG_CRIT | LOG_USER, \
"%s:%d: " FMT, __FUNCTION__, __LINE__, ## ARG)
#define ULOG_ERR_F(FMT, ARG...) syslog(LOG_ERR | LOG_USER, \
"%s:%d: " FMT, __FUNCTION__, __LINE__, ## ARG)
#define ULOG_WARN_F(FMT, ARG...) syslog(LOG_WARNING | LOG_USER, \
"%s:%d: " FMT, __FUNCTION__, __LINE__, ## ARG)
#define ULOG_INFO_F(FMT, ARG...) syslog(LOG_INFO | LOG_USER, \
"%s:%d: " FMT, __FUNCTION__, __LINE__, ## ARG)
#define ULOG_DEBUG_F(FMT, ARG...) syslog(LOG_DEBUG | LOG_USER, \
"%s:%d: " FMT, __FUNCTION__, __LINE__, ## ARG)

#define DLOG_CRIT(...) syslog(LOG_CRIT | LOG_DAEMON, __VA_ARGS__)
#define DLOG_ERR(...) syslog(LOG_ERR | LOG_DAEMON, __VA_ARGS__)
#define DLOG_WARN(...) syslog(LOG_WARNING | LOG_DAEMON, __VA_ARGS__)
#define DLOG_INFO(...) syslog(LOG_INFO | LOG_DAEMON, __VA_ARGS__)
#define DLOG_DEBUG(...) syslog(LOG_DEBUG | LOG_DAEMON, __VA_ARGS__)
#define DLOG_CRIT_L(FMT, ARG...) syslog(LOG_CRIT | LOG_DAEMON, \
"%s:%d: " FMT, __FILE__, __LINE__, ## ARG)
#define DLOG_ERR_L(FMT, ARG...) syslog(LOG_ERR | LOG_DAEMON, \
"%s:%d: " FMT, __FILE__, __LINE__, ## ARG)
#define DLOG_WARN_L(FMT, ARG...) syslog(LOG_WARNING | LOG_DAEMON, \
"%s:%d: " FMT, __FILE__, __LINE__, ## ARG)
#define DLOG_INFO_L(FMT, ARG...) syslog(LOG_INFO | LOG_DAEMON, \
"%s:%d: " FMT, __FILE__, __LINE__, ## ARG)
#define DLOG_DEBUG_L(FMT, ARG...) syslog(LOG_DEBUG | LOG_DAEMON, \
"%s:%d: " FMT, __FILE__, __LINE__, ## ARG)
#define DLOG_CRIT_F(FMT, ARG...) syslog(LOG_CRIT | LOG_DAEMON, \
"%s:%d: " FMT, __FUNCTION__, __LINE__, ## ARG)
#define DLOG_ERR_F(FMT, ARG...) syslog(LOG_ERR | LOG_DAEMON, \
"%s:%d: " FMT, __FUNCTION__, __LINE__, ## ARG)
#define DLOG_WARN_F(FMT, ARG...) syslog(LOG_WARNING | LOG_DAEMON, \
"%s:%d: " FMT, __FUNCTION__, __LINE__, ## ARG)
#define DLOG_INFO_F(FMT, ARG...) syslog(LOG_INFO | LOG_DAEMON, \
"%s:%d: " FMT, __FUNCTION__, __LINE__, ## ARG)
#define DLOG_DEBUG_F(FMT, ARG...) syslog(LOG_DEBUG | LOG_DAEMON, \
"%s:%d: " FMT, __FUNCTION__, __LINE__, ## ARG)

#define ULOG_OPEN(X) openlog(X, LOG_PID | LOG_NDELAY, LOG_USER)
#define DLOG_OPEN(X) openlog(X, LOG_PID | LOG_NDELAY, LOG_DAEMON)
#define LOG_CLOSE() closelog()

#elif defined OSSOLOG_STDOUT

/* log to the stdout */

#define ULOG_CRIT(...) (printf(__VA_ARGS__),printf("\n"))
#define ULOG_ERR(...) (printf(__VA_ARGS__),printf("\n"))
#define ULOG_WARN(...) (printf(__VA_ARGS__),printf("\n"))
#define ULOG_INFO(...) (printf(__VA_ARGS__),printf("\n"))
#define ULOG_DEBUG(...) (printf(__VA_ARGS__),printf("\n"))
#define ULOG_CRIT_L(FMT, ARG...) printf(\
"%s:%d: " FMT "\n", __FILE__, __LINE__, ## ARG)
#define ULOG_ERR_L(FMT, ARG...) printf(\
"%s:%d: " FMT "\n", __FILE__, __LINE__, ## ARG)
#define ULOG_WARN_L(FMT, ARG...) printf(\
"%s:%d: " FMT "\n", __FILE__, __LINE__, ## ARG)
#define ULOG_INFO_L(FMT, ARG...) printf(\
"%s:%d: " FMT "\n", __FILE__, __LINE__, ## ARG)
#define ULOG_DEBUG_L(FMT, ARG...) printf(\
"%s:%d: " FMT "\n", __FILE__, __LINE__, ## ARG)
#define ULOG_CRIT_F(FMT, ARG...) printf(\
"%s:%d: " FMT "\n", __FUNCTION__, __LINE__, ## ARG)
#define ULOG_ERR_F(FMT, ARG...) printf(\
"%s:%d: " FMT "\n", __FUNCTION__, __LINE__, ## ARG)
#define ULOG_WARN_F(FMT, ARG...) printf(\
"%s:%d: " FMT "\n", __FUNCTION__, __LINE__, ## ARG)
#define ULOG_INFO_F(FMT, ARG...) printf(\
"%s:%d: " FMT "\n", __FUNCTION__, __LINE__, ## ARG)
#define ULOG_DEBUG_F(FMT, ARG...) printf(\
"%s:%d: " FMT "\n", __FUNCTION__, __LINE__, ## ARG)

#define DLOG_CRIT(...) (printf(__VA_ARGS__),printf("\n"))
#define DLOG_ERR(...) (printf(__VA_ARGS__),printf("\n"))
#define DLOG_WARN(...) (printf(__VA_ARGS__),printf("\n"))
#define DLOG_INFO(...) (printf(__VA_ARGS__),printf("\n"))
#define DLOG_DEBUG(...) (printf(__VA_ARGS__),printf("\n"))
#define DLOG_CRIT_L(FMT, ARG...) printf(\
"%s:%d: " FMT "\n", __FILE__, __LINE__, ## ARG)
#define DLOG_ERR_L(FMT, ARG...) printf(\
"%s:%d: " FMT "\n", __FILE__, __LINE__, ## ARG)
#define DLOG_WARN_L(FMT, ARG...) printf(\
"%s:%d: " FMT "\n", __FILE__, __LINE__, ## ARG)
#define DLOG_INFO_L(FMT, ARG...) printf(\
"%s:%d: " FMT "\n", __FILE__, __LINE__, ## ARG)
#define DLOG_DEBUG_L(FMT, ARG...) printf(\
"%s:%d: " FMT "\n", __FILE__, __LINE__, ## ARG)
#define DLOG_CRIT_F(FMT, ARG...) printf(\
"%s:%d: " FMT "\n", __FUNCTION__, __LINE__, ## ARG)
#define DLOG_ERR_F(FMT, ARG...) printf(\
"%s:%d: " FMT "\n", __FUNCTION__, __LINE__, ## ARG)
#define DLOG_WARN_F(FMT, ARG...) printf(\
"%s:%d: " FMT "\n", __FUNCTION__, __LINE__, ## ARG)
#define DLOG_INFO_F(FMT, ARG...) printf(\
"%s:%d: " FMT "\n", __FUNCTION__, __LINE__, ## ARG)
#define DLOG_DEBUG_F(FMT, ARG...) printf(\
"%s:%d: " FMT "\n", __FUNCTION__, __LINE__, ## ARG)

#define ULOG_OPEN(X) do {} while (0)
#define DLOG_OPEN(X) do {} while (0)
#define LOG_CLOSE() do {} while (0)

#else 

/* log to the stderr */

#define ULOG_CRIT(...) (fprintf(stderr, __VA_ARGS__),fprintf(stderr, "\n"))
#define ULOG_ERR(...) (fprintf(stderr, __VA_ARGS__),fprintf(stderr, "\n"))
#define ULOG_WARN(...) (fprintf(stderr, __VA_ARGS__),fprintf(stderr, "\n"))
#define ULOG_INFO(...) (fprintf(stderr, __VA_ARGS__),fprintf(stderr, "\n"))

#ifdef OSSOLOG_DEBUG
#define ULOG_DEBUG(...) (fprintf(stderr, __VA_ARGS__),fprintf(stderr, "\n"))
#else
#define ULOG_DEBUG(...) do {} while (0)
#endif

#define ULOG_CRIT_L(FMT, ARG...) fprintf(stderr, \
"%s:%d: " FMT "\n", __FILE__, __LINE__, ## ARG)
#define ULOG_ERR_L(FMT, ARG...) fprintf(stderr, \
"%s:%d: " FMT "\n", __FILE__, __LINE__, ## ARG)
#define ULOG_WARN_L(FMT, ARG...) fprintf(stderr, \
"%s:%d: " FMT "\n", __FILE__, __LINE__, ## ARG)
#define ULOG_INFO_L(FMT, ARG...) fprintf(stderr, \
"%s:%d: " FMT "\n", __FILE__, __LINE__, ## ARG)
#define ULOG_DEBUG_L(FMT, ARG...) fprintf(stderr, \
"%s:%d: " FMT "\n", __FILE__, __LINE__, ## ARG)
#define ULOG_CRIT_F(FMT, ARG...) fprintf(stderr, \
"%s:%d: " FMT "\n", __FUNCTION__, __LINE__, ## ARG)
#define ULOG_ERR_F(FMT, ARG...) fprintf(stderr, \
"%s:%d: " FMT "\n", __FUNCTION__, __LINE__, ## ARG)
#define ULOG_WARN_F(FMT, ARG...) fprintf(stderr, \
"%s:%d: " FMT "\n", __FUNCTION__, __LINE__, ## ARG)
#define ULOG_INFO_F(FMT, ARG...) fprintf(stderr, \
"%s:%d: " FMT "\n", __FUNCTION__, __LINE__, ## ARG)

#ifdef OSSOLOG_DEBUG
#define ULOG_DEBUG_F(FMT, ARG...) fprintf(stderr, \
"%s:%d: " FMT "\n", __FUNCTION__, __LINE__, ## ARG)
#else
#define ULOG_DEBUG_F(...) do {} while (0)
#endif

#define DLOG_CRIT(...) (fprintf(stderr, __VA_ARGS__),fprintf(stderr, "\n"))
#define DLOG_ERR(...) (fprintf(stderr, __VA_ARGS__),fprintf(stderr, "\n"))
#define DLOG_WARN(...) (fprintf(stderr, __VA_ARGS__),fprintf(stderr, "\n"))
#define DLOG_INFO(...) (fprintf(stderr, __VA_ARGS__),fprintf(stderr, "\n"))

#ifdef OSSOLOG_DEBUG
#define DLOG_DEBUG(...) (fprintf(stderr, __VA_ARGS__),fprintf(stderr, "\n"))
#else
#define DLOG_DEBUG(...)
#endif

#define DLOG_CRIT_L(FMT, ARG...) fprintf(stderr, \
"%s:%d: " FMT "\n", __FILE__, __LINE__, ## ARG)
#define DLOG_ERR_L(FMT, ARG...) fprintf(stderr, \
"%s:%d: " FMT "\n", __FILE__, __LINE__, ## ARG)
#define DLOG_WARN_L(FMT, ARG...) fprintf(stderr, \
"%s:%d: " FMT "\n", __FILE__, __LINE__, ## ARG)
#define DLOG_INFO_L(FMT, ARG...) fprintf(stderr, \
"%s:%d: " FMT "\n", __FILE__, __LINE__, ## ARG)
#define DLOG_DEBUG_L(FMT, ARG...) fprintf(stderr, \
"%s:%d: " FMT "\n", __FILE__, __LINE__, ## ARG)
#define DLOG_CRIT_F(FMT, ARG...) fprintf(stderr, \
"%s:%d: " FMT "\n", __FUNCTION__, __LINE__, ## ARG)
#define DLOG_ERR_F(FMT, ARG...) fprintf(stderr, \
"%s:%d: " FMT "\n", __FUNCTION__, __LINE__, ## ARG)
#define DLOG_WARN_F(FMT, ARG...) fprintf(stderr, \
"%s:%d: " FMT "\n", __FUNCTION__, __LINE__, ## ARG)
#define DLOG_INFO_F(FMT, ARG...) fprintf(stderr, \
"%s:%d: " FMT "\n", __FUNCTION__, __LINE__, ## ARG)

#ifdef OSSOLOG_DEBUG
#define DLOG_DEBUG_F(FMT, ARG...) fprintf(stderr, \
"%s:%d: " FMT "\n", __FUNCTION__, __LINE__, ## ARG)
#else
#define DLOG_DEBUG_F(...) do {} while (0)
#endif

#define ULOG_OPEN(X) do {} while (0)
#define DLOG_OPEN(X) do {} while (0)
#define LOG_CLOSE() do {} while (0)

#endif /* !defined OSSOLOG_STDOUT && !defined OSSOLOG_STDERR */

#else

/* no logging is compiled to the binary */

#define ULOG_CRIT(...) do {} while (0)
#define ULOG_ERR(...) do {} while (0)
#define ULOG_WARN(...) do {} while (0)
#define ULOG_INFO(...) do {} while (0)
#define ULOG_DEBUG(...) do {} while (0)
#define ULOG_CRIT_L(...) do {} while (0)
#define ULOG_ERR_L(...) do {} while (0)
#define ULOG_WARN_L(...) do {} while (0)
#define ULOG_INFO_L(...) do {} while (0)
#define ULOG_DEBUG_L(...) do {} while (0)
#define ULOG_CRIT_F(...) do {} while (0)
#define ULOG_ERR_F(...) do {} while (0)
#define ULOG_WARN_F(...) do {} while (0)
#define ULOG_INFO_F(...) do {} while (0)
#define ULOG_DEBUG_F(...) do {} while (0)

#define DLOG_CRIT(...) do {} while (0)
#define DLOG_ERR(...) do {} while (0)
#define DLOG_WARN(...) do {} while (0)
#define DLOG_INFO(...) do {} while (0)
#define DLOG_DEBUG(...) do {} while (0)
#define DLOG_CRIT_L(...) do {} while (0)
#define DLOG_ERR_L(...) do {} while (0)
#define DLOG_WARN_L(...) do {} while (0)
#define DLOG_INFO_L(...) do {} while (0)
#define DLOG_DEBUG_L(...) do {} while (0)
#define DLOG_CRIT_F(...) do {} while (0)
#define DLOG_ERR_F(...) do {} while (0)
#define DLOG_WARN_F(...) do {} while (0)
#define DLOG_INFO_F(...) do {} while (0)
#define DLOG_DEBUG_F(...) do {} while (0)

#define ULOG_OPEN(X) do {} while (0)
#define DLOG_OPEN(X) do {} while (0)
#define LOG_CLOSE() do {} while (0)

#endif /* OSSOLOG_COMPILE */

#endif /* OSSO_LOG_H */