#!/bin/sh
qmake -project -o lang.pro
lupdate lang.pro
lrelease gui/lang/wengo_cs.ts
lrelease gui/lang/wengo_de.ts
lrelease gui/lang/wengo_en.ts
lrelease gui/lang/wengo_es.ts
lrelease gui/lang/wengo_fr.ts
lrelease gui/lang/wengo_it.ts
lrelease gui/lang/wengo_ja.ts
lrelease gui/lang/wengo_nl.ts
lrelease gui/lang/wengo_pl.ts
lrelease gui/lang/wengo_pt.ts
lrelease gui/lang/wengo_ru.ts
lrelease gui/lang/wengo_sk.ts
lrelease gui/lang/wengo_sv.ts
lrelease gui/lang/wengo_vi.ts
lrelease gui/lang/wengo_zh-cn.ts
lrelease gui/lang/wengo_zh.ts
