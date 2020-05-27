static void
fibonacci(Monitor *m, int s)
{
	unsigned int i, n;
	int nx, ny, nw, nh;
	Client *c;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);
	if (n == 0)
		return;
	if (n == 1) {
		c = nexttiled(m->clients);
		resize(c, m->wx, m->wy, m->ww - 2 * c->bw, m->wh - 2 * c->bw, 0);
		return;
	}

	nx = m->wx + m->gappx;
	ny = m->gappx;
	nw = m->ww - 2*m->gappx;
	nh = m->wh - 2*m->gappx;

	for (i = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next)) {
		if ((i % 2 && nh / 2 > 2*c->bw)
		   || (!(i % 2) && nw / 2 > 2*c->bw)) {
			if (i < n - 1) {
				if (i % 2)
					nh = (nh - m->gappx) / 2;
				else
					nw = (nw - m->gappx) / 2;

				if ((i % 4) == 2 && !s)
					nx += nw + m->gappx;
				else if ((i % 4) == 3 && !s)
					ny += nh + m->gappx;
			}
			if ((i % 4) == 0) {
				if (s)
					ny += nh + m->gappx;
				else
					ny -= nh + m->gappx;
			}
			else if ((i % 4) == 1)
				nx += nw + m->gappx;
			else if ((i % 4) == 2)
				ny += nh + m->gappx;
			else if ((i % 4) == 3) {
				if (s)
					nx += nw + m->gappx;
				else
					nx -= nw + m->gappx;
			}
			if (i == 0)	{
				if (n != 1)
					nw = (m->ww - 2*m->gappx - m->gappx) * m->mfact;
				ny = m->wy + m->gappx;
			}
			else if (i == 1)
				nw = m->ww - nw - m->gappx - 2*m->gappx;
			i++;
		}

		resize(c, nx, ny, nw - (2*c->bw), nh - (2*c->bw), False);
	}
}

void
dwindle(Monitor *mon) {
	fibonacci(mon, 1);
}

void
spiral(Monitor *mon) {
	fibonacci(mon, 0);
}
