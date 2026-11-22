# Pedido da rodada 4 de pesquisa (passo B0.5)

> Texto lançado na ferramenta de pesquisa em 22/11/2026. O texto fica em inglês porque o pedido é processado em inglês; o relatório volta em português. Quando o relatório for incorporado ao `BASE_MODELOS` e ao registro de fontes, este arquivo sai do repositório.

## Escopo resumido
1. **Parte A.** Completar ano, título, autoria, URL oficial e página dos 31 itens da fila de verificação de `fontes.json`.
2. **Parte B.** Lacunas da `BASE_MODELOS` §10 (Ky derivado de experimento brasileiro, nutrição da alface nos manuais estaduais, regador, pH–V, cinética do calcário, DT50 no Brasil, descompactação por braquiária, milheto e guandu, fatores da EUPS, Cousens e parâmetros das daninhas, INMET horário, fontes da fase 2, exemplos numéricos do FAO-56 e do TR-55).
3. **Parte C.** Componentes reaproveitáveis do APSIM (licença, SoilWater, SoilNitrogen, SurfaceOrganicMatter, MicroClimate, SoilArbitrator, PMF, daninhas, dados de validação).
4. **Parte D.** Verificações da `ARQUITETURA_SIMULACAO` §12.
5. **Parte E.** Referências de interface e de acessibilidade para o documento 13.
6. **Parte F.** Realismo e validação da base de clima e solo sem cultura (grade climática diária, INMET, temperatura do solo, umidade observada, biologia e química do solo, bases de solos, métricas de aceite).
7. **Parte G.** Apoio ao fluxo de ingestão de cultura (escala BBCH, estrutura dos Sistemas de Produção da Embrapa, dados abertos do Agrofit, parâmetros por grupo de cultura e piloto da beterraba).

## Texto do pedido

```text
RESEARCH TASK — "simulador-agro", round 4: close the remaining data gaps, complete the metadata of 31 queued sources, extract reusable components from APSIM, and verify architecture/UI claims. Final report in Brazilian Portuguese (technical jargon may stay in English).

PROJECT CONTEXT
simulador-agro is a 3D farming simulator / serious game (Godot 4.7.2 + a deterministic C++ core) for technical micro-management of a vegetable plot, bed by bed, for all of Brazil. Vertical slice: one flat lettuce (alface) bed, full cycle. Every agronomic number in the content pack must cite a registered publication with page/table/section. Source priority: (1) Embrapa; (2) other Brazilian public bodies, universities and journals (IAC, INMET, MAPA/Agrofit, IBAMA, ANA, state fertilizer manuals, RBCS, PAB, Bragantia, Horticultura Brasileira, Planta Daninha, RBEAA, Ciência Rural, Semina, Irriga); (3) reliable international sources (FAO, peer-reviewed journals, USDA/NRCS, EU FOCUS, APSIM documentation), flagged provisional. Process models are deterministic expected-value models; randomness only for game events. Daily time step; FAO-56 single bucket + Curve Number; AquaCrop-lite plant model in phase 1; APSIM-style resource arbitration; exact integer mass ledger.

HOW TO SEARCH EMBRAPA (open-access PDFs)
- BDPA: https://www.bdpa.cnptia.embrapa.br/consulta/busca?b=ad&busca=<TERM>&qFacets=<TERM>&biblioteca=vazio&sort=&paginacao=t&paginaAtual=1 (URL-encoded Portuguese term; facets Biblioteca, Autor, Assunto, Tipo do material; per-record detail and PDF links). Example: "descompactação" returns 94 records, e.g. Kochhann, Denardin & Berton, "Compactação e descompactação de solos", Embrapa Trigo Documentos 19 (2000); Drescher et al. 2016, PAB 51(2):159-168.
- Infoteca-e: https://www.infoteca.cnptia.embrapa.br/ (simple search /infoteca/simple-search?query=<TERM>).
- Alice: https://www.alice.cnptia.embrapa.br/ (simple search /alice/simple-search?query=<TERM>).
- Ainfo: https://ainfo.cnptia.embrapa.br/
Prefer official Embrapa URLs over mirrors.

PART A — Complete metadata of queued sources: exact title, full authors, year, journal/series with volume and pages (or unit and series number), official https URL, licence, and the exact page/table/figure of each value used.
A1 Marouelli & Oliveira et al., "Manejo da água de irrigação", cap. 5 of "Irrigação e fertirrigação em fruteiras e hortaliças" (alice doc/915574): year, authors, page of lettuce Kc 0.70/1.05/0.95, wetted-area factor 0.95, tension table.
A2 Marouelli, Embrapa Hortaliças Circular Técnica 57 "Tensiômetros para o controle de irrigação em hortaliças": year, official URL, Table 1 (10–20 kPa lettuce; 10 cm; 15 kPa example).
A3 Marouelli, Circular Técnica 69 (Irrigas): exact title, year, page with 15/25/40 kPa.
A4 Embrapa cartilha "Controle da irrigação de hortaliças usando tensiômetro com manômetro de vácuo" (infoteca doc/939283): year, authors, page.
A5 Fageria, Embrapa Arroz e Feijão, "Calagem e Adubação" (infoteca doc/1015113, p59.pdf): year, book, page of the 3-month/6-month lime reaction sentence.
A6 Ikeda, "Manejo integrado de plantas daninhas em olericultura" (Embrapa): year, unit, formal publication with lettuce critical periods (PAI 19–21, PTPI 34–35 days).
A7 Embrapa "Rotação e sucessão de culturas como práticas de conservação do solo no município de Jales, SP" (ainfo 138887): year, authors, page with the LS equation.
A8 Raij, Cantarella & Zullo 1979, Bragantia 38(7):57–69, doi 10.1590/S0006-87051979000100007: exact title, page of pH(água) = 4.50 + 0.025·V.
A9 Brunini et al. 1976, Bragantia 35(1):213–219 (lettuce base temperature, White Boston): title, authors, URL, page.
A10 Fernandes et al. 2019, Rev. Bras. Meteorologia 34(4):505–514 (lettuce thermal time, Bom Jesus-PI): title, authors, URL, table with 898 and 1050 °C·day.
A11 Sartori, Lombardi Neto & Genovez 2005, RBRH parts 1 and 2: titles, volume, pages, URLs, adapted CN table.
A12 Embrapa Circular Técnica 136 (application efficiency by irrigation method): unit, title, year, URL, table.
A13 ClimaBR: Zanetti, Oliveira & Pruski, "CLIMABR parte I" (RBEAA) and validation in Engenharia Agrícola 26(1):96–102 (2006): titles, years, pages, URLs.
A14 RBCS 39:1723–1736 (2015) RE/PRNT tables: title, authors, URL.
A15 FAO Irrigation and Drainage Paper 66 (Steduto et al. 2012): official URL, cabbage Ky table/page.
A16 Lettuce Ky studies: Kuslu et al. (SJAR, ky 1.39); Acta Hortic. 1335 article 84 (1.17); Gesunde Pflanzen 2022 doi 10.1007/s10343-022-00744-w (2.26 and 0.69); Univ. Pitești paper-22 (0.88); unheated greenhouse cv. Olenka (1.096): titles, authors, years, volumes, pages.
A17 IntechOpen chapter 73381 "Modeling of Nitrogen Use Efficiency in Lettuce Culture (Lactuca sativa): Isotopic Nitrogen (15N) and AquaCrop": authors, year, book, parameter table.
A18 Agronomy 16(14):1328 (2026) Songjiang lettuce deficit irrigation + organic fertilizer: title, authors.
A19 Suárez-Rey et al. 2025, Ann. Appl. Biol. doi 10.1111/aab.70019: title, authors, volume, pages.
A20 Abreu, Reichert & Reinert 2004 RBCS 28:519–531; Reinert et al. 2008 RBCS 32:1805–1816 (confirm density limits 1.75/1.85 Mg m-3 and page); Cunha et al. 2011 RBCS 35:589–602: titles, authors.
A21 Williams & Weil 2004 SSSAJ 68(4):1403–1409; Chen & Weil 2010 Plant Soil 331:31–43 (RP threshold crossed by radish roots, if stated); Cresswell & Kirkegaard 1995 Aust. J. Soil Res. 33(2):221–239: titles.
A22 Floresta 39(2):381–391 (2009) LS transcription: title, authors; ideally a primary transcription of Bertoni & Lombardi Neto "Conservação do Solo" with LS and the P-factor table.
A23 APSIM SoilWat documentation (cn_red, cn_rain, CNCov, CNRed): version/date; Cichota et al. 2021 MethodsX 8:101566: title.
A24 "Intervalo hídrico ótimo de um Latossolo Vermelho distrófico após o primeiro período de pastejo contínuo de Brachiaria ruziziensis em sistema integração lavoura-pecuária" (RP = 0.0123·θ^-1.2762·Ds^5.6262): authors, journal, year, volume, pages, URL.
A25 Nicoloso et al. 2008 RBCS 32(4):1723–1734 (exact title, authors); Tormena, Silva & Libardi 1998 RBCS 22:573–581 (SciELO URL and Busscher coefficients a, b, c with units); Giancotti, Machado & Yamauti 2011 Semina (volume, number, pages, URL).

PART B — Remaining data gaps (values with units, exact location, Brazilian vs international, quality, caveats; never invent numbers; mark DERIVED values).
B1 Brazilian lettuce deficit-irrigation experiments with yield and ETa per treatment (derive Ky); lettuce dry-matter fraction, SLA, LAI vs cover, commercial HI (Horticultura Brasileira, Ciência Rural); heat thresholds for tropical lettuce (bolting, tipburn), cardinal temperatures of Brazilian crisp/tropical cultivars (Embrapa Hortaliças).
B2 N, P2O5, K2O recommendations and target V for lettuce in IAC Boletim 100 (2022), CQFS-RS/SC (2016), MG 5ª Aproximação (1999), IPA-PE (2008), Embrapa Hortaliças; nutrient export per ton or hectare.
B3 Watering can / hose application efficiency or uniformity.
B4 pH–V relationships for other Brazilian mineralogies/regions; pH(CaCl2) ↔ pH(água) regressions.
B5 Lime dissolution kinetics by particle size (Brazilian incubation studies, e.g. Merlotto IAC; international).
B6 DT50 measured in Brazilian/tropical soils for pesticides registered for lettuce/vegetables; Brazilian reviews; PPDB terms of use; Agrofit data access.
B7 Biological decompaction by Urochloa, pearl millet, pigeon pea, Crotalaria (Calonego & Rosolem; Silveira Junior et al. 2012 RBCS 36:1854–1867; Foloni et al. 2006); persistence of biopores after the cover crop; Drescher et al. 2016 PAB 51(2):159–168 as mechanical reference.
B8 Brazilian USLE factors: P table, C for vegetables/bare soil, K for major soil classes.
B9 Cousens I and A for lettuce; growth parameters (Tb, RUE, k, max height, rooting depth) for Bidens pilosa, Amaranthus spp., Portulaca oleracea, Cyperus rotundus, Galinsoga parviflora, Eleusine indica, Digitaria spp.; APSIM weed parameter sets; hydrothermal-time emergence parameters.
B10 INMET hourly automatic-station data access, variables, format, terms; Parton & Logan 1981 parameters and Brazilian calibrations; Hargreaves–Samani calibrations in Brazil; ClimaBR availability.
B11 Phase 2: Embrapa composting guidance; drip tape emitter flow/spacing for lettuce beds (Embrapa Hortaliças); cherry tomato Embrapa parameters (Kc, cycle, spacing).
B12 FAO-56 worked examples with full inputs/outputs for unit tests (Examples 8, 9, 10, 11, 17, 18 with Δ, γ, es, ea, Rn, Ra and the 3.9 mm/day result; Hargreaves) and the NRCS TR-55 CN runoff example; exact fao.org page URLs.

PART C — APSIM reuse (user request)
C1 Licence of APSIM Next Generation (ApsimX): can an MIT-licensed public game reuse code or only re-implement published algorithms? Required attribution.
C2 Core equations, state variables, default parameters and documentation URLs for: SoilWater (cascading layers, CN with cover and tillage, Ritchie two-stage evaporation U/CONA, SWCON drainage, runoff); SoilNitrogen/Nutrient (FOM pools, BIOM, HUM, INERT, rates, temperature and moisture factors, nitrification, denitrification, urea hydrolysis); SurfaceOrganicMatter; MicroClimate (light partition among canopies); SoilArbitrator (water/N arbitration among crops); Plant Modelling Framework (thermal-time phenology, organ arbitration, Leaf/SimpleLeaf, roots); APSIM weed models; any APSIM vegetable or lettuce model.
C3 Confirm Keating et al. 2003, European Journal of Agronomy 18:267–288, section 3.9: crop modules communicate only via the engine with resource modules.
C4 APSIM validation datasets or outputs usable as regression tests for a re-implementation.

PART D — Architecture verification
D1 Gomes et al. 2018 "Co-simulation: a survey", ACM Computing Surveys 51(3): exact statements on Gauss–Seidel vs Jacobi stability and fixed-point iteration.
D2 Linköping ECP 157 (Modelica) paper: fixed-step Gauss–Jacobi/Gauss–Seidel masters inadequate for discontinuous signals (title, authors, quote).
D3 arXiv 1705.09704 "Lock-step simulation is child's play": transcendental functions across platforms.
D4 Godot 4.x: do GDScript math functions call the platform C library? Official Windows build flags and x86-64 baseline? godot-cpp with custom flags (-ffp-contract=off) under MinGW; recommended godot-cpp branch for Godot 4.7.
D5 Priority-Flood (Barnes, Lehman & Mulla 2014, Computers & Geosciences) and flat resolution (Barnes et al. 2014): references, DOIs, open versions, deterministic tie-breaking.
D6 Williams 1975 MUSLE (units, original reference); Fiedler "Fix Your Timestep!".
D7 Benchmarks GDScript vs C++ GDExtension for numeric loops in Godot 4.x.

PART E — UI/UX
E1 Medieval Dynasty (official): fertility, fertilizer, watering, weeds, days per season, sleep, farming UI.
E2 Farming Simulator 25 official manual (Giants): HUD, time scale, sleep, seasons, soil sampling UI, help.
E3 Agricultural serious games for education (including Brazilian/Embrapa); learning outcomes, explanatory feedback, debriefing.
E4 Game Accessibility Guidelines and Xbox Accessibility Guidelines: text size, colour-blind design, remapping, subtitles for a PC game designed at 1280×720.

PART F — Climate and soil base: realism and validation without crops (the climate and soil models are the foundation; they must reproduce real Brazilian climates and soils for any future crop)
F1 Gridded daily weather for any Brazilian municipality: Xavier, King & Scanlon BR-DWGD (Brazilian Daily Weather Gridded Data): latest version, period, variables (Tmax, Tmin, Rs, RH, u2, P, ET0), resolution, access, licence, validation papers; alternatives (NASA POWER, ERA5-Land) and their known biases in Brazil.
F2 INMET automatic vs conventional stations: hourly variables, portal/API, terms of use; stations with long series in five reference regions (semi-arid PE/BA, Cerrado GO/DF/MG, Southeast SP, South RS/PR, Amazon AM/PA).
F3 Soil temperature models for daily soil simulation (APSIM SoilTemp/Campbell, DSSAT/EPIC soil temperature, Parton 1984, damping-depth models): equations, inputs, parameters, and Brazilian validation studies; Brazilian soil temperature observation datasets (INMET conventional stations, Embrapa agrometeorological stations, research datasets).
F4 Observed soil water dynamics under bare soil/fallow in Brazil for validation: lysimeter, TDR or neutron-probe studies with soil water content, drainage and evaporation (Embrapa Cerrados, Embrapa Semiárido, IAC, ESALQ); flux towers in Brazil (LBA, FLUXNET) as ET references; measured parameters of two-stage soil evaporation in Brazil.
F5 Soil biology: soil organic matter pool models calibrated or applied in Brazil (Century, RothC, DayCent, APSIM SoilNitrogen); N mineralization incubation data for Brazilian soils (potentially mineralizable N, rates, temperature and moisture responses); soil respiration data; decomposition rates of common residues in Brazil (vegetable residues, cover crops, compost, manure).
F6 Soil chemistry dynamics for validation: pH, V and Al over time after liming in Brazilian field or incubation experiments; P sorption (P-remanescente, Langmuir) by soil class; K leaching; nitrate leaching in vegetable beds in Brazil.
F7 Brazilian soil property databases by class (Embrapa BDsolos, PronaSolos, SiBCS profiles: texture, density, organic carbon, CEC, pH) for parameterizing scenario soils; access and licence.
F8 Validation metrics and acceptance thresholds used in soil and crop model evaluation (Willmott d, RMSE, NSE, bias), especially thresholds adopted in Brazilian model evaluations.

PART G — Support for the crop ingestion pipeline (the agent will read sets of PDFs about a crop, e.g. beet, and turn them into a validated content pack: anatomy, growth stages, diseases, pests, fungicides, herbicides, irrigation and management)
G1 BBCH scale: general scale and crop-specific scales for lettuce, beet, tomato and leafy vegetables; official publication (Meier, JKI/BBA monograph), terms of use, Portuguese usage in Brazil.
G2 How Embrapa structures crop information ("Sistemas de Produção" standard sections); AGROVOC and Crop Ontology identifiers for crops, pests and diseases.
G3 Agrofit (MAPA) open data: access (dados.gov.br, API), fields per registered product (crop, target, dose, safety interval, class), licence.
G4 Default parameters by crop group for missing values: FAO-56 crop groups and tables; AquaCrop crop types (leafy, root/tuber, fruit, grain) and their default or conservative parameters; APSIM crop classes.
G5 Beet (beterraba) as the pilot: Embrapa Hortaliças publications (cultivation system, Kc, cycle, base temperature or thermal time, spacing, nutrition, main pests and diseases such as Cercospora leaf spot, registered products) and Brazilian studies; international AquaCrop or crop-model parameters for beet, noting the difference between table beet and sugar beet.

OUTPUT
1. PART A table: proposed ID (format ORG-YEAR-ACRONYM, keep given IDs), complete reference, official URL, year, licence, exact location of each value.
2. PART B summary table (RESOLVED-BR / RESOLVED-PROVISIONAL-INTL / PARTIAL / NOT FOUND) with recommended default and source, then details.
3. PART C component-by-component guide with licence implications and phase recommendation.
4. PART D, E, F and G findings with URLs and design implications (for F, propose the five reference scenarios with data sources and acceptance metrics).
5. Registry-ready bibliography of every new source (ID, reference, https URL, domain, type, year, licence, access date).
6. Unresolved items and next step for each.
```
