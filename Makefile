NAME := xre-dev
SHELL := /bin/bash

.DEFAULT_GOAL := up

up down:
	docker compose $@ 

shell:
	docker compose exec $(NAME) $(SHELL)

re: down up

.PHONY: up down re shell