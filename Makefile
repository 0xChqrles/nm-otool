NM_NAME		=	ft_nm
OTOOL_NAME	=	ft_otool
SHEL		=	/bin/bash

LIBFT_DIR	=	libft/
NM_DIR		=	nm_srcs/
OTOOL_DIR	=	otool_srcs/

.PHONY : all clean

all :
	@make --no-print-directory -C $(LIBFT_DIR)
	@make --no-print-directory -C $(NM_DIR)
	@cp $(addprefix $(NM_DIR), $(NM_NAME)) ./
	@make --no-print-directory -C $(OTOOL_DIR)
	@cp $(addprefix $(OTOOL_DIR), $(OTOOL_NAME)) ./

clean :
	@make clean --no-print-directory -C $(LIBFT_DIR)
	@make clean --no-print-directory -C $(NM_DIR)
	@make clean --no-print-directory -C $(OTOOL_DIR)

fclean :
	@rm -rf $(NM_NAME)
	@rm -rf $(OTOOL_NAME)
	@make fclean --no-print-directory -C $(LIBFT_DIR)
	@make fclean --no-print-directory -C $(NM_DIR)
	@make fclean --no-print-directory -C $(OTOOL_DIR)

re : fclean all
