/**
 * This example shows how to add elements to the
 * tree structure created. The tree structure is the
 * same used to hold all the resource
 *
 * When adding a child, the algorithm doesn't look for cyclic
 * branchs! Be careful!
 */

#include "internal/tree.hpp"
#include <cstdlib>
#include <cstdio>

using branch = typename CoAP::branch<int>::branch;

void print_branch(branch& br, int level = 0) noexcept
{
	std::printf("%d: %d:[", level, br.value());

	branch* n;

	for(unsigned i = 0; (n = br[i]); i++)
	{
		if(i != 0) std::printf(" ");
		std::printf("%d", n->value());
	}
	std::printf("]\n");

	++level;
	for(unsigned i = 0; (n = br[i]); i++)
	{
		print_branch(*n, level);
	}
}

int main()
{
	std::printf("init...\n");
	branch na{1}, nb{2}, nc{3}, nd{4}, ne{5}, nf{6};

	/**
	 * Adding using add_child;
	 */
	na.add_child(nb, nd);
	nb.add_child(nc, ne);
	ne.add_child(nf);

	auto* n = na.find_child(2);
	if(n)
	{
		std::printf("found %d\n", n->value());
	}
	else
	{
		std::printf("not found\n");
	}

	auto* p = na.find_descendant(2, 5, 6);
	if(p)
	{
		std::printf("found %d\n", p->value());
	}
	else
	{
		std::printf("not found\n");
	}

	std::printf("Printing...\n");
	print_branch(na);

	std::printf("\n");
	branch ca{1}, cb{2}, cc{3}, cd{4}, ce{5}, cf{6}, cg{7}, ch{8}, ci{9}, cj{10},
			ck{11}, cl{12}, cm{13}, cn{14}, co{15}, cp{16}, cq{17}, cr{18}, cs{19}, ct{20},
			cu{21}, cv{22}, cx{23}, cw{24}, cy{25}, cz{26};

	std::printf("Adding branch\n");
	/**
	 * Adding using add_branch
	 */
	ca.add_branch<false>(cb, cc, cd, ce, cf, cg, ch, ci, cj);
	cc.add_branch<false>(ck, cl);
	ca.add_branch<false>(cb, cc, cd, cm);
	ca.add_branch<false>(cb, cn, co, cp);
	ca.add_branch<false>(cb, cc, cd, ce, cf, cg, cr, cs, ct, cu, cv, cx, cw, cy, cz);

	print_branch(ca);

	return EXIT_SUCCESS;
}
