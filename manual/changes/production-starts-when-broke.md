---
title: Start production regardless of funds
category: fix
release: 0.1.0
targets: []
---

Ordering something a house cannot yet afford now starts its build clock, which then creeps
forward as money arrives, instead of leaving the order dormant. Production used to begin
only if the house could pay the first installment, and an order placed while short of funds
stayed suspended even after the money came in, so the player had to notice and click the
icon again. The build itself already tolerated running out of money part way through by
holding its place until the house could pay.
