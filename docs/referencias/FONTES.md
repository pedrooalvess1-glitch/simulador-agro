# Registro de fontes

> Como registrar uma publicação em `fontes.json`. Política de fontes: `00_INDICE` §1.4. Formato do dado com fonte no pacote: `CONVENCOES` §4.

## 1. Campos de cada fonte

| Campo | Tipo | Descrição |
|---|---|---|
| `id` | texto | Identificador fixo (`CONVENCOES` §1), ex.: `EMB-CNPH-2020-ALFACE`, `RBCS-2008-NICOLOSO` |
| `titulo` | texto | Título completo da publicação |
| `autoria` | texto | Autores, unidade ou órgão e, para artigo, periódico, volume e páginas |
| `tipo` | texto | Circular Técnica, Comunicado Técnico, Documentos, Sistema de Produção, Boletim de Pesquisa, Boletim técnico, Lei, Cartilha, Livro, Capítulo, Artigo, Revisão, Manual técnico, Guia regulatório, Base de dados, Artigo e base de dados, Licença de software, Documentação de software, Página de software, Código-fonte, Artigo técnico (blog), Artigo (revista técnica), Guia de projeto, Wiki de comunidade |
| `ano` | inteiro | Ano de publicação |
| `url` | texto | Endereço https da publicação, num domínio de `dominios_permitidos` |
| `acesso` | data | Data em que o conteúdo foi conferido (AAAA-MM-DD) |
| `licenca` | texto | Licença ou declaração de direitos encontrada na leitura (§5). "Licença não localizada" quer dizer uso só por citação |

Campos opcionais são `regiao` (onde o dado vale), `culturas` (lista de IDs), `notas` e `pendencias` (lista do que falta conferir, como página de tabela ou título completo).

## 2. Fila de verificação
Publicação achada por pesquisa e ainda sem os dados mínimos (ano, título exato, URL oficial) entra em `fila_de_verificacao`, com `id_proposto`, `referencia`, `url` quando houver, `falta` e `etapa` (passo do `MAPA_DO_PROJETO` que precisa dela). Item da fila não pode ser citado em `[FONTE:ID]` nem no pacote. Quando os dados chegam, o item sai da fila e entra em `fontes`. O ID proposto é corrigido nessa hora se a publicação original mostrar outro ano ou outro autor.

## 3. Regras
1. Só entra no registro publicação aberta e lida pelo agente ou por uma rodada de pesquisa do projeto. Nada de referência de memória.
2. O ID não muda depois de citado. Publicação revisada ganha ID novo com o ano novo. A exceção é ID com ano ou autor errado. Ele é corrigido no mesmo PR que troca todas as citações.
3. Quando uma citação secundária diverge da publicação original, vale a original. A divergência fica anotada na `BASE_MODELOS` (decisão 58).
4. O PDF baixado fica em `docs/referencias/pdf/<id>.pdf`, pasta fora do Git.
5. Domínio novo em `dominios_permitidos` segue a política do `00_INDICE` §1.4. O usuário aprovou fontes brasileiras não Embrapa e fontes internacionais confiáveis (decisão 38).
6. O CI roda `tools/conferir_fontes.py`. Registro inválido, citação a ID inexistente ou citação a item da fila barram o PR.

## 4. Licença e condição de uso
1. O projeto nunca redistribui a publicação. O PDF fica fora do Git (`CLAUDE.md` §3, regra 4) e o registro guarda só a referência.
2. O projeto usa números, equações e métodos, sempre com autor e origem. No Brasil o conteúdo científico ou técnico não é protegido (art. 7º, § 3º), métodos e conceitos matemáticos também não (art. 8º, I), e a citação com autor e origem é livre (art. 46, III) [FONTE:BR-1998-LEI9610]. Por isso toda fonte do registro pode ser usada desse jeito, qualquer que seja a licença.
3. Texto, tabela inteira ou figura de uma publicação só entram no repositório se a licença permitir (CC BY, domínio público). Com "direitos reservados" ou "licença não localizada", só a citação curta e os valores.
4. Código de terceiros só entra com licença compatível com a MIT do repositório. O código do APSIM fica fora (decisão 55).
5. O verificador barra o texto "a conferir" no campo `licenca`.

## 5. Como buscar na Embrapa
1. **BDPA.** `https://www.bdpa.cnptia.embrapa.br/consulta/busca?b=ad&busca=<termo>&qFacets=<termo>&biblioteca=vazio&sort=&paginacao=t&paginaAtual=1`. A lista tem facetas por unidade, autor, assunto e tipo de material e link direto para o PDF aberto.
2. **Infoteca-e.** `https://www.infoteca.cnptia.embrapa.br/` guarda as publicações seriadas (Circular Técnica, Comunicado Técnico, Documentos).
3. **Alice.** `https://www.alice.cnptia.embrapa.br/` guarda artigos e capítulos de pesquisadores da Embrapa.
4. Se a busca não responder pela nuvem, o agente usa o navegador do notebook do usuário (U3).
