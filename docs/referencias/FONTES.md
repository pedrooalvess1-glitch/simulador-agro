# Registro de fontes

> Como registrar uma publicação em `fontes.json`. Política de fontes: `00_INDICE` §1.4. Formato do dado com fonte no pacote: `CONVENCOES` §4.

## 1. Campos de cada fonte

| Campo | Tipo | Descrição |
|---|---|---|
| `id` | texto | Identificador fixo (`CONVENCOES` §1), ex.: `EMB-CNPH-2020-ALFACE` |
| `titulo` | texto | Título completo da publicação |
| `autoria` | texto | Autores e unidade da Embrapa |
| `tipo` | texto | Sistema de Produção, Circular Técnica, Comunicado Técnico, Documentos, Livro, Boletim de Pesquisa |
| `ano` | inteiro | Ano de publicação |
| `url` | texto | Endereço https da publicação, num domínio de `dominios_permitidos` |
| `acesso` | data | Data em que o agente conferiu o conteúdo (AAAA-MM-DD) |
| `licenca` | texto | Licença ou condição de reprodução; "a conferir" até o passo B2 do `ROTEIRO` |

Campos opcionais: `regiao` (onde a recomendação vale), `culturas` (lista de IDs), `notas`.

## 2. Regras
1. Só entra no registro publicação que o agente abriu e leu. Nada de referência de memória.
2. O ID nunca muda depois de citado; publicação revisada ganha ID novo com o ano novo.
3. O PDF baixado fica em `docs/referencias/pdf/<id>.pdf`, pasta fora do Git.
4. Domínio novo em `dominios_permitidos` (ex.: órgão público para lacuna da Embrapa) só com decisão do usuário (`00_INDICE` §9, pergunta 1).
5. O CI roda `tools/conferir_fontes.py`; registro inválido ou citação a ID inexistente barra o PR.
