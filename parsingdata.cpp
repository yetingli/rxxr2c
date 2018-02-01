#include "parsingdata.h"

//Create regex with default metadata
struct regex *make_r(struct exp *e, int i, int j) {
  struct regex *r;
  r = new struct regex;
  struct meta *m;
  m = new struct meta;
  m->spos = i;
  m->epos = j;
  m->scount = 0;
  m->nullable = 0;
  r->expression = e;
  r->metadata = m;
  return r;
}

struct ctr *make_ctr(int i, int j, struct ctr *c, struct ctr *d) {
  struct ctr *r = new struct ctr;
  r->a = i;
  r->b = j;
  r->lt = c;
  r->rt = d;
  return r;
}

struct ctr *ctr_add_r(struct ctr *node, int u, int v) {
  if(node == NULL)
    return make_ctr(u, v, NULL, NULL);
  else if(v < node->a) {
    struct ctr *t = ctr_add_r(node->lt, u, v);
    node->lt = t;
    return node;
  }
  else if(node->b < u) {
    struct ctr *t = ctr_add_r(node->rt, u, v);
    node->rt = t;
    return node;
  }
  else {
    if(node->a != u) {
      struct ctr *t = ctr_add_r(node->lt, min(node->a, u), zprev(max(node->a, u)));
      node->lt = t;
    }
    if(node->b != v) {
      struct ctr *t = ctr_add_r(node->rt, znext(min(node->b, v)), max(node->b, v));
      node->rt = t;
    }
    struct ctr *t = make_ctr(max(node->a, u), min(node->b, v), node->lt, node->rt);
    return t;
  }
}

struct ctr *ctr_add(struct ctr *node, int u, int v) {
  if(node->a > node->b)
    return ctr_add_r(node, v, u);
  else
    return ctr_add_r(node, u, v);
}

struct llist<struct pair<char> *> *ctr_positive_r(struct ctr *node, struct llist<struct pair<char> *> *lst) {
  if(node == NULL)
    return lst;
  struct llist<struct pair<char> *> *t = ctr_positive_r(node->rt, lst);
  if(t == NULL) {
    struct pair<char> *p = new pair<char>;
    p->a = node->a; p->b = node->b;
    struct llist<struct pair<char> *> *r = new llist<struct pair<char> *>;
    r->head = p; r->tail = NULL;
    return ctr_positive_r(node->lt, r);
  }
  else {
    if(znext(node->b) == t->head->a) {
      t->head->a = node->a;
      return ctr_positive_r(node->lt, t);
    }
    else {
      struct pair<char> *p = new pair<char>;
      p->a = node->a; p->b = node->b;
      struct llist<struct pair<char> *> *r = new llist<struct pair<char> *>;
      r->head = p; r->tail = t;
      return ctr_positive_r(node->lt, r);
    }
  }
}

struct llist<struct pair<char> *> *ctr_positive(struct ctr *node) {
  return ctr_positive_r(node, NULL);
}

struct neg_r {
  struct llist<struct pair<char> *> *a;
  char b;
};

struct neg_r *mkNegR(struct llist<struct pair<char> *> *i, char j) {
  struct neg_r *r = new struct neg_r;
  r->a = i;
  r->b = j;
  return r;
}

struct neg_r *ctr_negative_r(struct ctr *node, struct llist<struct pair<char> *> *neg, char next) {
  if(node == NULL) {
    struct neg_r *r = mkNegR(neg, next);
    return r;
  }
  struct neg_r *t = ctr_negative_r(node->rt, neg, next);
  if(node->b == zprev(t->b))
    return ctr_negative_r(node->lt, t->a, node->a);
  else {
    struct pair<char> *p = new pair<char>;
    p->a = znext(node->b); p->b = zprev(t->b);
    struct llist<struct pair<char> *> *r = new struct llist<struct pair<char> *>;
    r->head = p; r->tail = neg;
    return ctr_negative_r(node->lt, r, node->a);
  }
}

struct llist<struct pair<char> *> *ctr_negative(struct ctr *node) {
  struct neg_r *r = ctr_negative_r(node, NULL, '\x80');
  if(ZMIN < r->b) {
    struct pair<char> *t = new pair<char>;
    t->a = ZMIN; t->b = zprev(r->b);
    struct llist<struct pair<char> *> *r2 = new struct llist<struct pair<char> *>;
    r2->head = t; r2->tail = r->a;
    return r2;
  }
  else
    return r->a;
}

struct *exp makeZero() {
  struct exp *r = new struct exp;
  r->type = Zero;
  return r;
}

struct *exp makeOne() {
  struct exp *r = new struct exp;
  r->type = One;
  return r;
}

struct *exp makeDot() {
  struct exp *r = new struct exp;
  r->type = Dot;
  return r;
}

struct *exp makePred(enum predt pr') {
  struct exp *r = new struct exp;
  r->type = Pred;
  r->pr = pr';
  return r;
}

struct *exp makeAtomChar(char c1) {
  struct exp *r = new struct exp;
  r->type = Atom;
  r->at = new struct atom;
  r->at->isList = 0;
  r->at->c1 = c1;
  r->at->next = NULL;
  return r;
}

struct *exp makeAtomClass(char c1, char c2, struct atom *next) {
  struct exp *r = new struct exp;
  r->type = Atom;
  r->at = new struct atom;
  r->at->c1 = c1;
  r->at->c2 = c2;
  r->at->next = next;
  return r;
}

struct *exp makeGroup() {
  struct exp *r = new struct exp;
  r->type = Group;
  r->gk = new struct group;
}

struct *exp makeBackref()

struct *exp makeConc

struct *exp makeAlt

struct *exp makeKleene

