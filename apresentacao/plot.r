library(ggplot2)
library(dplyr)

argumentos <- commandArgs(trailingOnly = TRUE)
caminho_csv <- argumentos[1]
prefixo_saida <- argumentos[2]

dados_benchmark <- read.csv(caminho_csv)

resumo <- dados_benchmark |>
  group_by(estrutura, tamanho) |>
  summarise(media = mean(media_passagens), .groups = "drop")

grafico_base <- ggplot(resumo, aes(x = tamanho, y = media, color = estrutura)) +
  geom_line(linewidth = 1) +
  geom_point() +
  labs(x = "Quantidade de elementos", y = "Passagens (média por busca)", color = "Estrutura") +
  theme_minimal()

ggsave(paste0(prefixo_saida, "-linear.png"), grafico_base, width = 8, height = 5, dpi = 150)

grafico_log <- grafico_base + scale_x_log10() + scale_y_log10()
ggsave(paste0(prefixo_saida, "-loglog.png"), grafico_log, width = 8, height = 5, dpi = 150)