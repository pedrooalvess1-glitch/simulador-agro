# NN — Título do documento

> Modelo dos documentos de planejamento (`00_INDICE` §5). Copie para `docs/NN_NOME_EM_MAIUSCULAS.md` (`CONVENCOES` §1), troque o título, preencha as seções na ordem e apague estas linhas de instrução.
> Regras para todas as seções: todo número agronômico com fonte registrada e local (`CONVENCOES` §4); toda simplificação rotulada [MODELO] e toda decisão de projeto [INFERÊNCIA]; só o estado atual, com o mínimo de texto (`CLAUDE.md` §3, regra 8). Termos: `GLOSSARIO_DO_PROJETO.md`.

**Estado:** rascunho | pronto · **Esforço:** `médio` | `alto` | `extra` (`ROTEIRO` §2)

## 1. Objetivo
> Uma ou duas frases: que pergunta do projeto este documento responde e quem o usa.

## 2. Escopo e limites
> O que está dentro e, em lista separada, o que fica fora (e onde isso é tratado).

## 3. Fontes envolvidas
> **Campo obrigatório.** Uma linha por publicação do registro que o documento usa. Se o documento é genérico, escreva "Nenhuma: o conteúdo entra pelo pacote (documento 14)".

| ID da fonte | O que fornece aqui | Rótulo |
|---|---|---|

## 4. Dependências
> Documentos que precisam estar prontos antes e documentos que dependem deste.

## 5. Contratos e esquemas
> Formatos de dados, sinais, funções públicas. Esquema em JSON Schema ou tabela campo · tipo · obrigatório · descrição.

## 6. Decisões
> Cada decisão com o mecanismo que a justifica, as alternativas descartadas e a origem. Decisão de projeto também entra no `DECISOES.md`.

| Decisão | Mecanismo / motivo | Alternativas descartadas | Origem |
|---|---|---|---|

## 7. Exemplos
> Ao menos um exemplo completo de cada contrato da §5, com dado real citado quando houver.

## 8. Casos de teste
> Formato: dado → ação → resultado esperado. Para o modelo, o resultado esperado vem de valor publicado, com fonte.

## 9. Perguntas abertas
> Só as que o agente não pode decidir sozinho (P1), cada uma com a opção recomendada.

## 10. Critérios de pronto
- [ ] Todas as seções preenchidas; nenhuma instrução do modelo restante.
- [ ] §3 completa; `tools/conferir_fontes.py` sem erro.
- [ ] Toda simplificação rotulada [MODELO] e toda decisão [INFERÊNCIA].
- [ ] Perguntas abertas respondidas ou registradas como pendência explícita.
- [ ] Links internos válidos (CI).
