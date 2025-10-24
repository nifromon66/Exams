/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vbc.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nifromon <nifromon@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/24 16:54:56 by nifromon          #+#    #+#             */
/*   Updated: 2025/10/24 16:54:57 by nifromon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VBC_H
#define VBC_H

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>


void unexpected(char c);
int ft_factor();
int ft_product();
int ft_sum();
int check_input(char *str);

#endif