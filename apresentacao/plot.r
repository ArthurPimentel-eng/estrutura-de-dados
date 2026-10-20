library(ggplot2)
library(dplyr)
library(patchwork)

argumentos <- commandArgs(trailingOnly = TRUE)
caminho_csv <- argumentos[1]
prefixo_saida <- argumentos[2]

dados_benchmark <- read.csv(caminho_csv)
stopifnot(nrow(dados_benchmark) > 0)

rotulos_numericos <- scales::label_number(big.mark = ".", decimal.mark = ",")

resumo <- dados_benchmark |>
  group_by(estrutura, tamanho) |>
  summarise(media = mean(media_passagens), .groups = "drop")

dados_lista <- filter(resumo, estrutura == "lista")
dados_arvore <- filter(resumo, estrutura == "arvore")

grafico_lista <- ggplot(dados_lista, aes(x = tamanho, y = media)) +
  geom_line(linewidth = 1, color = "#00BFC4") +
  geom_point(color = "#00BFC4") +
  scale_x_continuous(labels = rotulos_numericos) +
  scale_y_continuous(labels = rotulos_numericos) +
  labs(title = "Lista encadeada", x = "Quantidade de elementos", y = "Passagens (média por busca)") +
  theme_minimal()

grafico_arvore <- ggplot(dados_arvore, aes(x = tamanho, y = media)) +
  geom_smooth(method = "lm", formula = y ~ log(x), se = FALSE, linewidth = 1, color = "#F8766D") +
  geom_point(size = 2, color = "#F8766D") +
  scale_x_continuous(labels = rotulos_numericos) +
  scale_y_continuous(labels = rotulos_numericos) +
  labs(title = "Árvore binária de busca", x = "Quantidade de elementos", y = "Passagens (média por busca)") +
  theme_minimal()

grafico_combinado <- grafico_lista + grafico_arvore

ggsave(paste0(prefixo_saida, "-combinado.png"), grafico_combinado, width = 11, height = 4.8, dpi = 150)

expoente_lista <- coef(lm(log10(media) ~ log10(tamanho), data = dados_lista))[2]
coeficiente_arvore <- coef(lm(media ~ log2(tamanho), data = dados_arvore))[2]

cat("Expoente da lista (log-log):", round(expoente_lista, 3), "\n")
cat("Coeficiente da árvore (por log2 n):", round(coeficiente_arvore, 3), "\n")