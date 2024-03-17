// #include "xdp.h"
// #include "log.h"
// #include "file.h"
// #include "xleaks.h"
// #include "hexxer.h"
// #include "modes.h"
// #include <stdlib.h>
// #include <stdbool.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <unistd.h>

// bool xre_fd_consume(int fd, t_hexxer *hexxer, t_modes mode)
// {
//     ssize_t ret;
// 	bool inf;
// 	size_t n = hexxer->max_size;
// 	size_t offset = hexxer->start_offset;

// 	if (n == 0)
// 		inf = true;
// 	else
// 		inf = false;

// 	if (offset)
// 	{
// 		struct stat st;

// 		if (fstat(fd, &st) < 0) {
// 			__log__(error, "%s", ERROR_MSG);
//         	goto prison;
// 		}
// 		/*
// 		 *     if the file is regular we can just
// 		 *     lseek to the desired offset, if not, we
// 		 *     move the cursor by reading and discarding
// 		 *     data until the offset is met
// 		*/
// 		if (S_ISREG(st.st_mode))
// 		{
// 			ret = lseek(fd, offset, SEEK_CUR);
// 			if (ret == -1) {
// 				__log__(error, "%s", ERROR_MSG);
// 				goto prison;
// 			}

// 		} else
// 		{
// 			char 	buf[4096];
// 			size_t  i = offset;
// 			ssize_t ret;

// 			while (i)
// 			{
// 				ret = read(fd, buf, i < 4096 ? i : 4096);
// 				if (ret == -1) {
// 					__log__(error, "%s", ERROR_MSG);
// 					goto prison;
// 				}
// 				i -= ret;
// 			}
// 		}
// 	}

// 	while (666)
// 	{
// 		size_t rd_size;

// 		if (!inf)
// 		{
// 			if (!n)
// 				break;
// 			if (n < hexxer->data.cap)
// 				rd_size = n;
// 			else
// 				rd_size = hexxer->data.cap;
// 		}
// 		else
// 			rd_size = hexxer->data.cap;

// 		ret = read(fd, (ptr_t)hexxer->data.ptr, rd_size);
// 		switch (ret)
// 		{
// 			case -1:
// 				__log__(error, "%s", ERROR_MSG);
// 				goto prison;
// 			case  0:
// 				goto beach;
// 		}

//     	switch (mode)
//     	{
//     	    case XDP_REGULAR:
//     	        xd_dump_lines(
// 			 	   hexxer->data.ptr,
// 			 	   ret,
// 			 	   offset,
// 			 	   hexxer->screen.ptr,
// 			 	   hexxer->screen.size,
// 				   hexxer->color
//     	        ); break;
//     	    case XDP_STREAM:
//     	        xd_dump_hex_stream(
// 			       hexxer->data.ptr,
// 			       ret,
// 			       offset,
// 			       hexxer->screen.ptr,
// 			       hexxer->screen.size
//                 ); break;
// 			case XDP_STRINGS:
//     	     	xd_dump_strings(
// 		 	       hexxer->data.ptr,
// 		 	       ret,
// 		 	       hexxer->screen.ptr,
// 		 	       hexxer->screen.size
//     	        ); break;
//     	    default:
// 				break;
//     	}
// 		offset += ret;

// 		if (!inf)
// 			n -= ret;
// 	}

// beach:
// 	return (true);

// prison:
// 	return (false);
// }